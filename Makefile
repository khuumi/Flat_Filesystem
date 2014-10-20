
CC  = g++
CXX = g++

INCLUDES =
CFLAGS   = -g -Wall $(INCLUDES)
CXXFLAGS = -g -Wall $(INCLUDES)

LDFLAGS = 
LDLIBS =

executables = objput objget objlist objgetacl objsetacl objtestacl
objects = objput.o objget.o objlist.o objgetacl.o objsetacl.o objtestacl.o tools.o


# ifneq "$(strip $(userfile))" ""
# .PHONY: exec
# 	exec: build
# 	@echo init to $(userfile)
# 	@#echo put your logic for initialization to $(userfile) here
# else
# exec: build
# 	@echo default_init
# 	@#echo put your default initialization logic here
# endif

exec: make_users
	build


.PHONY: default

make_users:
	mkdir flat_fs_repo
	chown -R flat_fs flat_fs_repo
	chmod 700 -R flat_fs_repo
	FILE=$usernames
	
	while IFS=" " read -r -a input; do
		user="$${input[0]}"
  		useradd $$user
    for i in "$${input[@]:1}" do
    	groupadd $$i
    	echo "Added group $$i"
    	usermod -a -G $$i $$user 
    	echo "Added $user to $$i"
    done

	done < $FILE


default: $(executables)

$(executables): tools.o
$(objects): tools.h

build: all

test: build
	echo $(userfile) > userfile.proxy

	echo "Testing our program\n"
	@echo "------------"

	echo "this is a test" > newfile
	./objput doc < newfile 
	@echo "------------"

	./objget doc
	@echo "this is a test [EXPECTED]"

	./objlist
	@echo "doc [EXPECTED]"
	@echo "------------"

	./objgetacl doc
	@echo "u1.* rwxpv [EXPECTED]"
	@echo "------------"

	echo "u1.* r" > newacl
	./objsetacl doc < newacl
	@echo "------------"

	./objtestacl -a r doc
	@echo "allowed [EXPECTED]"
	@echo "------------"

	./objtestacl -a x doc
	@echo "denied [EXPECTED]"
	@echo "------------"

	rm newfile newacl 

.PHONY: clean
clean:
	rm -f *.o *~ a.out core $(objects) $(executables) /flat_fs_repo/u*

.PHONY: all
all: clean default


