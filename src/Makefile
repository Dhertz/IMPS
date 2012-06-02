CC      = gcc
CFLAGS  = -Wall -g -pedantic -std=c99

.SUFFIXES: .c .o
.PHONY: all clean

#make all
all: assemble emulate

#build executables
emulate: emulate.o
assemble: assemble.o symtable.o

#Rules for .o files	
emulate.o: emulate.c
assemble.o: assemble.c symtable.h
symtable.o: symtable.c symtable.h

clean:
	rm -f *.o
	rm -f assemble
	rm -f emulate
	rm -f symtable
