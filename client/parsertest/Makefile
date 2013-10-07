CC = g++
DEBUG = -g
#CFLAGS = -Wall $(DEBUG)
CFLAGS = -Wall
COMPILE = $(CC) $(CFLAGS) -c
LINK = $(CC) $(CFLAGS)

all: parsertest

parsertest: parsertest.o
	$(LINK) ../obj/aptcli.o ../obj/yum.o ../obj/MacSU.o ../obj/PackageManager.o ../obj/helpers.o parsertest.o -o parsertest

parsertest.o: parsertest.cpp
	$(COMPILE) -o $@ $<
clean:
	rm parsertest
