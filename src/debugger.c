#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include "debugger.h"

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
	
	/* START DEBUGGER */
	
	printf("Welcome to the IMPS debugger. Type h for help.\n");
	char cmd[10];
	readCommand(cmd, sizeof(cmd));
	printf("%s\n", cmd);
	
	/* END DEBUGGER */
    
    freeTable(&symbols);
    
    fclose(in);
    free(buffer2);
    return EXIT_SUCCESS;
}
