CC = g++
DEBUG = -g
CFLAGS = -Wall $(DEBUG)
COMPILE = $(CC) $(CFLAGS) -c

OBJFILES := $(patsubst %.cpp,%.o,$(wildcard *.cpp))

all: forest-client

forest-client: $(OBJFILES)
	$(CC) -o forest-client $(OBJFILES)

%.o: %.cpp
	$(COMPILE) -o $@ $<
clean:
	rm *.o forest-client
