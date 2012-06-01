#include <stdlib.h>
#include <stdio.h>
#include <string.h>
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
	printf("%i", size);
	
	buffer = malloc(size * 4);
	fread(buffer, 4, size, in);
	
	table symbols;
	init(&symbols);
	const char *delim = " ";
	char *token = strtok(buffer, delim);
	int offset = 0;
	
	while(offset < size) {
		printf("%s - %i\n", token, offset);
		if(strchr(token, ':') != NULL) {
			insertFront(&symbols, token, offset);
		}
		offset += strlen(token);
		token = strtok(NULL, delim);
	}
	
	for(iterator i = start(&symbols); i != end(&symbols); i = next(i)) {
		printf("%s -> %i\n", getKey(i), get(&symbols, getKey(i)));
	}

	freeTable(&symbols);
	
	fclose(in);
	free(buffer);

 	return EXIT_SUCCESS;
	
}
