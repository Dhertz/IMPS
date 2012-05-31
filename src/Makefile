CC      = gcc
CFLAGS  = -Wall -pedantic -std=c99

.SUFFIXES: .c .o

.PHONY: all clean

all: assemble emulate



#build executable for emulate
emulate: emulate.o

#Rules for emulate's .o files	
emulate.o: emulate.c


clean:
	rm -f *.o
	rm -f assemble
	rm -f emulate
