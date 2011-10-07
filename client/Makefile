CC = g++
DEBUG = -g
CFLAGS = -Wall $(DEBUG)
COMPILE = $(CC) $(CFLAGS) -c
LINK = $(CC) $(CFLAGS)

OBJFILES := $(patsubst %.cpp,%.o,$(wildcard *.cpp))

all: forest-client

forest-client: $(OBJFILES)
	$(LINK) $(OBJFILES) -lcurl -o forest-client

%.o: %.cpp
	$(COMPILE) -o $@ $<
clean:
	rm *.o forest-client
