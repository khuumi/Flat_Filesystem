CC = gcc
CXX = g++

INCLUDES =
CFLAGS = -g -Wall $(INCLUDES)
CXXFLAGS = -g -Wall $(INCLUDES)

LDFLAGS = -g
LDLIBS = -lssl -lcrypto

example:
	gcc example.c -c example.o $(LDLIBS)



test: build
	./tests/test1.sh
	./tests/test2.sh
	./tests/test3.sh

.PHONY: exec
	
ifneq "$(strip $(userfile))" ""
exec: clean build
	./scripts/initsecurefs.sh $(userfile)
else
exec: clean build
	./scripts/initsecurefs.sh users.txt
endif	

.PHONY: clean
clean:
	rm -f *.o *~ $(executables)
	-./scripts/clean.sh $(userfile)
