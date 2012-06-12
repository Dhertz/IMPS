#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include "symtable.h"
#include "utils.h"

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
	buildSymTable(&symbols, in, size, buffer);
	
    free(buffer);
	
    /* Set up output file */
    FILE *out;
    if ((out = fopen(argv[2], "wb")) == NULL) {
        perror("fopen");
        exit(EXIT_FAILURE);
    }
    
    /* Second pass - create binary instructions */
	const char *delim = "\n";
    char *state;
    char *token = strtok_r(buffer2, delim, &state);
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
    free(buffer2);
    return EXIT_SUCCESS;    
}
