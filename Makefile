CC=gcc
CFLAGS=-ggdb -O2 -W -Wall -pedantic `pkg-config gtk+-2.0 --cflags` -std=c99 -Wno-unused-parameter
LIBS=-lSDL -lpthread `curl-config --libs` `pkg-config gtk+-2.0 --libs`

all: gtkPview

gtkPview: main.c main.h
	$(CC) $(CFLAGS) -c main.c
	$(CC) main.o $(LIBS) -o gtkPview

.PHONY: clean

clean:
	rm -rf *.o
	rm -rf main gtkPview

clean-all:
	rm -rf logo.*
