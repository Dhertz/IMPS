CC      = gcc
CFLAGS  = -Wall -g -pedantic -std=c99
#Your short username. Prompts for password if ssh keys not set up.
USER    = dh611
#Your Working directory with the IMPS and autotest repos.
DIR     = Dropbox/FirstYearWork/Programming/cproject/
.SUFFIXES: .c .o
.PHONY: all clean test

#make all
all: assemble emulate debug

#make and run tests
test: assemble emulate
	ssh $(USER)@fusion24.doc.ic.ac.uk /homes/$(USER)/$(DIR)/IMPS/src/makescript.sh $(USER) $(DIR)

#build executables
emulate: emulate.o symtable.o utils.o
assemble: assemble.o symtable.o utils.o
debug: debug.o symtable.o utils.o

#Rules for .o files	
emulate.o: emulate.c utils.h
assemble.o: assemble.c utils.h
debug.o: debug.c utils.h
symtable.o: symtable.c symtable.h
utils.o: utils.c utils.h

clean:
	rm -f *.o
	rm -f assemble
	rm -f emulate
	rm -f debug
