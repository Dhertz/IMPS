CC      = gcc
CFLAGS  = -Wall -pedantic -std=c99
USER    = dh611
DIR     = Dropbox/FirstYearWork/Programming/cproject/IMPS/src

.SUFFIXES: .c .o
.PHONY: all clean

#make all
all: assemble emulate
	ssh $(USER)@fusion24.doc.ic.ac.uk /homes/$(USER)/$(DIR)/makescript.sh

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
