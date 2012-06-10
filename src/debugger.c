#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include "debugger.h"
#define MAX_COMMAND_LENGTH 30

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
	
    /* Create copy of buffer for use in second pass */
    buffer2 = malloc(size * 4);
    memcpy(buffer2, buffer, size);
    
    /* First pass - fill symbol table with labels -> offsets */
    table symbols;
    init(&symbols);
	int numLines = buildSymTable(&symbols, in, size, buffer);
	
    free(buffer);
	
	/* Second pass - create binary instructions */
	const char *delim = "\n";
    char *state;
    char *token = strtok_r(buffer2, delim, &state);
	uint32_t *assembled = malloc(numLines * sizeof(uint32_t));
	
    int offset = 0;
	int emptylines = 0;
    
	/* Fill assembled with the binary instructions */
    while (token != NULL) {
        assembled[offset] = convertInstruction(token, symbols, offset);
        
        token = strtok_r(NULL, delim, &state);
        offset++;
        if (token != NULL) {
            /* Jump over lines with just newlines (or other ctrl characters) */
            if (strcmp(token, "\r") <= 0) {
                token = strtok_r(NULL, delim, &state);
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
	int* breaks = malloc(sizeof(int) * numLines);
	int breakCount = 0;
	
	bool running = false;
	
	/* Pre-run menu loop, for setting breakpoints */
	while (!running) {
		/* cmd = the whole input line */
		char cmd[MAX_COMMAND_LENGTH];
		readCommand(cmd, sizeof(cmd));
		
		/* cmdStr = the command keyword */
		char *rest;
		char *cmdStr = strtok_r(cmd, " ", &rest);
		int cmdCode = get(&commands, cmdStr);
		int line;
			
		switch (cmdCode) {
			case 0:
			/* quit - q */
				exit(EXIT_SUCCESS);
			break;
			case 1:
			/* break - b */
				line = atoi(strtok_r(NULL, " ", &rest));
				breaks[breakCount] = line;
				breakCount++;
			break;
			case 2:
			/* run - r */
				running = true;
			break;
			case 6:
			/* help - h */
				showHelp();
			break;
			default:
				printf("Invalid command - %s.\n", cmdStr);
		}
	}
	
	/*
	END DEBUGGER
	*/
    
	
    freeTable(&symbols);
    
    fclose(in);
    free(buffer2);
    return EXIT_SUCCESS;
}
