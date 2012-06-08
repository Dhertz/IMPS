CC      = gcc
CFLAGS  = -Wall -g -pedantic -std=c99
#Your short username. Prompts for password if ssh keys not set up.
USER    = tod11
#Your Working directory with the IMPS and autotest repos.
DIR     = temp
.SUFFIXES: .c .o
.PHONY: all clean test

#make all
all: assemble emulate

#make and run tests
test: assemble emulate
	ssh $(USER)@fusion24.doc.ic.ac.uk /homes/$(USER)/$(DIR)/IMPS/src/makescript.sh $(USER) $(DIR)

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