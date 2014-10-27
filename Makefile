
CC  = g++
CXX = g++

INCLUDES =
CFLAGS   = -g -Wall -std=c++11 $(INCLUDES)
CXXFLAGS = -g -Wall -std=c++11 $(INCLUDES)

LDFLAGS = 
LDLIBS =

executables = objput objget objlist objgetacl objsetacl objtestacl
objects = objput.o objget.o objlist.o objgetacl.o objsetacl.o objtestacl.o tools.o

.PHONY: default


.PHONY: exec	 
ifneq "$(strip $(userfile))" ""
exec: build init_scripts

@echo init to $(userfile)
#@#echo put your logic for initialization to $(userfile) here
else
userfile="usernames" 	
exec: build init_scripts
#@#echo put your default initialization logic here
endif


#exec: build init_scripts

init_scripts:
	- useradd flat_fs
	- mkdir flat_fs_repo
	chown -R flat_fs flat_fs_repo
	chmod 700 -R flat_fs_repo
	./make_users.sh $(userfile)

	chmod 4333 $(executables)



default: $(executables)

$(executables): tools.o
$(objects): tools.h

build: all 

test: build

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
	rm -rf *.o *~ a.out core $(objects) $(executables) flat_fs_repo
	./remove_users.sh $(userfile)

.PHONY: all
all: clean default


