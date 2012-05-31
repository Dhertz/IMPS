CC      = gcc
CFLAGS  = -Wall -pedantic -std=c99

.SUFFIXES: .c .o

.PHONY: all clean

all: assemble emulate symtable

#build executable for emulate
emulate: emulate.o
assemble: assemble.o
symtable: symtable.o

#Rules for emulate's .o files	
emulate.o: emulate.c
assemble.o: assemble.c
symtable.o: symtable.c

clean:
	rm -f *.o
	rm -f assemble
	rm -f emulate
	rm -f symtable
