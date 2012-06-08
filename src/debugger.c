#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include "debugger.h"

int main(int argc, char **argv) {
	FILE *in;
	long size;
    char *buffer = 0;

	/* Open and read from input file */
    if ((in = fopen(argv[1], "rb")) == NULL) {
        perror("fopen");
        exit(EXIT_FAILURE);
    }
    
    fseek(in, 0, SEEK_END);
    size = ftell(in);
    fseek(in, 0, SEEK_SET);
    
    /* First pass - fill symbol table with labels -> offsets */
    table symbols;
	int numlines = 0;
	//int *ptr = &numlines;
	
	symbols = buildSymTable(in, size, buffer);
	printf("%i\n", numlines);
	
	/* Second pass - create binary instructions */
	const char *delim = "\n";
    char *state;
    char *token = strtok_r(buffer, delim, &state);
	uint32_t *assembled = malloc(numlines * sizeof(uint32_t));
	
    int offset = 0;
	int emptylines = 0;
    
    while (token != NULL) {
        //assembled[offset] = convertInstruction(token, symbols, offset);
        
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

}
