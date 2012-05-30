CC      = gcc
CFLAGS  = -Wall -pedantic -std=c99

.SUFFIXES: .c .o

.PHONY: all clean

all: assemble emulate

assemble: assemble.o
	$(CC) $(CFLAGS) -o $@ $^

#build executable for emulate
emulate: emulate.o process.o

#Rules for emulate's .o files	
emulate.o: emulate.c emulate.h
process.o: process.c emulate.h

clean:
	rm -f *.o
	rm -f assemble
	rm -f emulate
