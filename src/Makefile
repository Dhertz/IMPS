CC      = gcc
CFLAGS  = -Wall -g -pedantic -std=c99
#Your short username. Prompts for password if ssh keys not set up.
USER    = tod11
#Your Working directory with the IMPS and autotest repos.
DIR     = Dropbox/Work/Year1/Programming/C/IMPS
.SUFFIXES: .c .o
.PHONY: all clean test

#make all
all: assemble emulate debugger

#make and run tests
test: assemble emulate
	ssh $(USER)@fusion24.doc.ic.ac.uk /homes/$(USER)/$(DIR)/IMPS/src/makescript.sh $(USER) $(DIR)

#build executables
debugger: debugger.o utils.o
emulate: emulate.o utils.o
assemble: assemble.o utils.o

#Rules for .o files	
debugger.o: debugger.c debugger.h
emulate.o: emulate.c debugger.h
assemble.o: assemble.c debugger.h
utils.o: utils.c debugger.h

clean:
	rm -f *.o
	rm -f assemble
	rm -f emulate
	rm -f debugger
