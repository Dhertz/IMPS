#include <stdlib.h>
#include <stdio.h>
#include "symtable.h"

int main(int argc, char **argv) {
	FILE *in;
	long size;
	char *buffer;
	
	if ((in = fopen(argv[1], "rb")) == NULL) {
		perror("fopen");
		exit(EXIT_FAILURE);
	}
	
	fseek(in, 0, SEEK_END);
	size = ftell(in);
	fseek(in, 0, SEEK_SET);
	
	buffer = malloc(size * 4);
	fread(buffer, 4, size, in);
	
	for (int i = 0; i < size; i++) {
		printf("%c", buffer[i]);
	}
	
	fclose(in);
	free(buffer);

 	return EXIT_SUCCESS;
	
}
