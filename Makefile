CC=gcc
CFLAGS=-O2 -W -Wall -pedantic `pkg-config gtk+-2.0 --cflags`
CFLAGS=-ggdb `pkg-config gtk+-2.0 --cflags`
LIBS=-lSDL -lpthread `curl-config --libs` `pkg-config gtk+-2.0 --libs`

main: main.c
	$(CC) $(CFLAGS) -c main.c
	$(CC) main.o $(LIBS) -o main

.PHONY: clean

clean:
	rm -rf *.o
	rm -rf main
	rm -rf logo.*
