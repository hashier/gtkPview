CC=gcc
CFLAGS=-ggdb -O2 -W -Wall -pedantic `pkg-config gtk+-2.0 --cflags` -std=c99
LIBS=-lSDL -lpthread `curl-config --libs` `pkg-config gtk+-2.0 --libs`

main: main.c main.h
	$(CC) $(CFLAGS) -c main.c
	$(CC) main.o $(LIBS) -o main

.PHONY: clean

clean:
	rm -rf *.o
	rm -rf main

clean-all:
	rm -rf logo.*
