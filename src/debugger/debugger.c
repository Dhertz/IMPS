#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>

int main(int argc, char **argv) {
    FILE *in;
    long size;
    
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
}