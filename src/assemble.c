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
    
    table symbols;
	symbols = buildSymTable(in, size, buffer);
	
    /* Set up output file */
    FILE *out;
    if ((out = fopen(argv[2], "wb")) == NULL) {
        perror("fopen");
        exit(EXIT_FAILURE);
    }
    
    /* Second pass - create binary instructions */
	const char *delim = "\n";
    char *state;
    char *token = strtok_r(buffer, delim, &state);
    int offset = 0;
    
    while (token != NULL) {
        uint32_t inst = convertInstruction(token, symbols, offset);
        
		fwrite(&inst, 4, 1, out);
		
        token = strtok_r(NULL, delim, &state);
        offset++;
        if (token != NULL) {
            /* Jump over lines with just newlines (or other ctrl characters) */
            if (strcmp(token, "\r") <= 0) {
                token = strtok_r(NULL, delim, &state);
            }
        }
    }
    
    freeTable(&symbols);
    
    fclose(in);
    fclose(out);
    free(buffer);
    return EXIT_SUCCESS;    
}
