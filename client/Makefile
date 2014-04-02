CC = g++
DEBUG = -g
#CFLAGS = -std=c++11 -pedantic -Wall $(DEBUG)
CFLAGS = -std=c++11 -pedantic -Wall
COMPILE = $(CC) $(CFLAGS) -c
LINK = $(CC) $(CFLAGS)

OBJFILES := $(patsubst %.cpp,obj/%.o,$(wildcard *.cpp))

all: bin/forest-client

bin/forest-client: $(OBJFILES)
	[ -d bin ] || mkdir bin
	$(LINK) $(OBJFILES) -lcurl -o bin/forest-client

obj/%.o: %.cpp
	[ -d obj ] || mkdir obj
	$(COMPILE) -o $@ $<
clean:
	rm obj/*.o bin/forest-client
