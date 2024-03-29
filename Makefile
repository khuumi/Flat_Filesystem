
CC  = g++
CXX = g++

INCLUDES =
CFLAGS   = -g -Wall -std=c++11 $(INCLUDES)
CXXFLAGS = -g -Wall -std=c++11 $(INCLUDES)

LDFLAGS = -g
LDLIBS = -lssl -lcrypto

executables = objput objget objlist objgetacl objsetacl objtestacl
objects = objput.o objget.o objlist.o objgetacl.o objsetacl.o objtestacl.o tools.o crypto.o

crypto:
	gcc crypto.c -c $(LDLIBS)

.PHONY: default exec

ifneq "$(strip $(userfile))" ""
exec: clean build init_scripts

@echo init to $(userfile)
else
userfile="usernames" 	
exec: clean build init_scripts
endif

init_scripts:
	- useradd flat_fs
	- mkdir /flat_fs_repo
	chown -R flat_fs /flat_fs_repo
	chmod 700 -R /flat_fs_repo
	./make_users.sh $(userfile)

	chmod 4001 $(executables)

default: crypto $(executables) 

$(executables): tools.o crypto.o
$(objects): tools.h crypto.h

build: default 

.PHONY: clean
clean:
	rm -rf *.o *~ a.out core $(objects) $(executables) /flat_fs_repo
	./remove_users.sh $(userfile)

.PHONY: all
all: clean default

.PHONY: test
test: exec
	./test1.sh
	./test2.sh
