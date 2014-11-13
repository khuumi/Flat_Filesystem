
CC  = g++
CXX = g++

INCLUDES = /usr/lib
CFLAGS   = -g -Wall -std=c++11 $(INCLUDES)
CXXFLAGS = -g -Wall -std=c++11 $(INCLUDES)

LDFLAGS = 
LDLIBS = -lssl

executables = new
objects = new.o

.PHONY: default

default: $(executables)

$(executables):
$(objects): 

build: all 

.PHONY: clean
clean:
	rm -rf *.o *~ a.out core $(objects) $(executables)

.PHONY: all
all: clean 

