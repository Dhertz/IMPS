#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include "symtable.h"
#include "utils.h"

#define MAX_COMMAND_LENGTH 30

/*
 * TODO:
 *
 * 1. Free st.mem somewhere
 * 2. Come up with better halt solution than exit(0) in utils.c:268
 *    Frees need to be accounted for (remember it's not just used by debugger)
 * 3. Static method for adding commands that takes long and short arguments
 * 4. Write showHelp
 * 5. Remove debug line on 169 of utils.c
 * 6. Dan's empty line hack?
 * 7. Fix break command I/O, reads 0 on Mac, sometimes reads negative number in DoC
 */

void readCommand(char buffer[], int size) {
    fgets(buffer, size, stdin);
	
    int i = 0;
    while (buffer[i] != '\0') {
        if (buffer[i] == '\n') {
            buffer[i] = '\0';
        } else {
            i++;
        }
    }
}

void addCommands(table *t) {
    insertFront(t, "quit", 0);
    insertFront(t, "q", 0);
    insertFront(t, "break", 1);
    insertFront(t, "b", 1);
    insertFront(t, "run", 2);
    insertFront(t, "r", 2);
    insertFront(t, "next", 3);
    insertFront(t, "n", 3);
    insertFront(t, "continue", 4);
    insertFront(t, "c", 4);
    insertFront(t, "print", 5);
    insertFront(t, "p", 5);
    insertFront(t, "help", 6);
    insertFront(t, "h", 6);
    insertFront(t, "setReg", 7);
    insertFront(t, "sR", 7);
    insertFront(t, "setAddr", 8);
    insertFront(t, "sA", 8);
    insertFront(t, "printReg", 9);
    insertFront(t, "pR", 9);
    insertFront(t, "printAddr", 10);
    insertFront(t, "pA", 10);
}

void showHelp() {
    printf("IMPS Debugger commands:\n\n");
    /* TODO: write these prints */
}

int main(int argc, char **argv) {
    FILE *in;
    long size;
    char *buffer;
    char *buffer2;

	printf("Reading input file %s... ", argv[1]);
	
    /* Open and read from input file */
    if ((in = fopen(argv[1], "rb")) == NULL) {
        perror("fopen");
        exit(EXIT_FAILURE);
    }
    
    fseek(in, 0, SEEK_END);
    size = ftell(in);
    fseek(in, 0, SEEK_SET);

    buffer = malloc(size * 4);
    fread(buffer, 4, size, in);
	fclose(in);
	printf("done.\n");

    /* Create copy of buffer for use in second pass */
    buffer2 = malloc(size * 4);
    memcpy(buffer2, buffer, size);
    
	printf("Building symbol table...");
	
    /* First pass - fill symbol table with labels -> offsets */
    table symbols;
    init(&symbols);
    int numLines = buildSymTable(&symbols, in, size, buffer);
     
    free(buffer);
    printf("done.\n");
	
    /* Second pass - create binary instructions */
    const char *delim = "\n";
    char *state;
    char *token = strtok_r(buffer2, delim, &state);
    state_t st = initState();
     
    int offset = 0;
    int emptylines = 0;
    int emptylinenos[emptylines];
	
    /* Fill assembled with the binary instructions */
    char **source = malloc(size * sizeof(char));
    if (source == NULL) {
        perror("malloc");
        exit(EXIT_FAILURE);
    }
	
    while (token != NULL) {
		/* Save token for printing at breakpoints */
        source[offset] = token;
		uint32_t inst = convertInstruction(token, symbols, offset, st);
        memcpy(st.mem + (offset * 4), &inst, sizeof(uint32_t));
        token = strtok_r(NULL, delim, &state);
        offset++;
        if (token != NULL) {
            /* Jump over lines with just newlines (or other ctrl characters) */
            if (strcmp(token, "\r") <= 0) {
                token = strtok_r(NULL, delim, &state);
                /* Track empty lines for correct line numbering later */
                emptylinenos[emptylines] = offset + 1;
                emptylines++;
            }
        }
    }
    

    /*
    START DEBUGGER
    */
    
    printf("Welcome to the IMPS debugger. Type \"h\" or \"help\" for information on commands.\n");
    
    /* Create symbol table to store synonomous commands */
    table commands;
    init(&commands);
    addCommands(&commands);
    
    /* Array to store line numbers of breakpoints */
    int* breaks = calloc(sizeof(int), numLines);
    int breakCount = 0;
    
    bool running = false;
    int line, regno, value, addr;
    char *rest, *addrString, *end;
	
    /* Pre-run menu loop, for setting breakpoints */
    while (!running) {
        /* cmd = the whole input line */
        char cmd[MAX_COMMAND_LENGTH];
        readCommand(cmd, sizeof(cmd));
		
        /* cmdStr = the command keyword */
        char *cmdStr = strtok_r(cmd, " ", &rest);
        int cmdCode = get(&commands, cmdStr);
        
        switch (cmdCode) {
            case 0:
				/* quit - q */
				printf("Quitting...\n");
				goto endDebugger;
            case 1:
				/* break - b */
                line = atoi(strtok_r(NULL, " ", &rest));
                int i = 0;
                while (emptylinenos[i] < line) {
					line--;
                }
                breaks[breakCount] = line;
                breakCount++;
				printf("\nBreakpoint set at line %i.\n", line);
				break;
            case 2:
				/* run - r */
                running = true;
				break;
            case 6:
				/* help - h */
                showHelp();
				break;
            case 7:
                /* setRegister - sR */
                regno = atoi(strtok_r(NULL, " ", &rest));
                value = atoi(strtok_r(NULL, " ", &rest));
                st.reg[regno] = value;
				break;
            case 8:
                /* setAddress - sA */
                addrString = strtok_r(NULL, " ", &rest);
                addr = strtol(addrString, &end, 16);
                value = atoi(strtok_r(NULL, " ", &rest));
                st.mem[addr] = value;
				break;
            default:
                printf("Invalid command - %s.\n", cmdStr);
        }
    }
	
    /* START EMULATING */
    int linecount = 1;
    while (st.halt == 0) {
        for (int i = 0; i < breakCount; i++) {
            if (linecount == breaks[i]) {
				/*Hacky emptyline adding back in*/
                int breakpt = line;
                while (emptylinenos[i] < breakpt) {
					breakpt++;
                }
				bool cont = false;
				
				printf("\nBreakpoint at line %i reached. What do you want to do?\n", breakpt);
				
				while (!cont) {
					printf("%i. %s\n", linecount, source[linecount - 1]);
					
					char cmd[MAX_COMMAND_LENGTH];
					readCommand(cmd, sizeof(cmd));
					char *rest;
					char *cmdStr = strtok_r(cmd, " ", &rest);
					int cmdCode = get(&commands, cmdStr);
				
					switch (cmdCode) {
						case 0:
							/* quit - q */
							printf("Quitting...\n");
							goto endDebugger;q
						case 3:
							/* next - n */
							st = executeInstruction(readUint32(st.pc, st), st);
							linecount++;
							break;
						case 4:
							/* continue - c */
							cont = true;
							break;
						case 5:
							/* print - p */
							printReg(st);
							break;
						case 6:
							/* help - h */
							showHelp();
							break;
						case 7:
							/* setRegister - sR */
							regno = atoi(strtok_r(NULL, " ", &rest));
							value = atoi(strtok_r(NULL, " ", &rest));
							st.reg[regno] = value;
							printf("$%i set to %i\n", regno, value);
							break;
						case 8:
							/* setAddress - sA */
							addrString = strtok_r(NULL, " ", &rest);
							addr = strtol(addrString, &end, 16);
							value = atoi(strtok_r(NULL, " ", &rest));
							st.mem[addr] = value;
							printf("mem[%i] set to %i\n", regno, value);
							break;
						case 9:
							/* printRegister - pR */
							regno = atoi(strtok_r(NULL, " ", &rest));
							printf("$%i = %i\n", regno, st.reg[regno]);
							break;
						case 10:
							/* printAddress - pA */
							addrString = strtok_r(NULL, " ", &rest);
							addr = strtol(addrString, &end, 16);
							printf("mem[%i] = %i\n", addr, st.mem[addr]);
							break;
						default:
							printf("Invalid command - %s.\n", cmdStr);
					}
				}
            }    
        }
        st = executeInstruction(readUint32(st.pc, st), st);
        linecount++;
    }
    
    /*
    END DEBUGGER
    */

    endDebugger:
		free(breaks);
        free(source);
        freeTable(&symbols);
        free(buffer2);
        return EXIT_SUCCESS;
}
