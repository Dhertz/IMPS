#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "symtable.h"

char *getLabel(char *str) {
	char *res = strtok_r(str, " ", &str);
	return res;
}

void addMnemonics(table *t) {
	char *mnemonics[] = {"halt", "add", "addi", "sub", "subi", "mul",
						 "muli", "lw", "sw", "beq", "bne", "blt", "bgt"
						 "ble", "jmp", "jr", "jal", "out"};
								
	for(int i = 0; i <= 19; i++) {
		insertFront(t, mnemonics[i], i);
	}
}

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
	
	table symbols;
	init(&symbols);
	
	const char *delim = "\n";
	char *token = strtok(buffer, delim);
	int offset = 0;
	
	while (token != NULL) {
		if (strchr(token, ':') != '\0') {
			insertFront(&symbols, getLabel(token), offset);
		}
		/* 0x00D = Carriage return - might need more coverage here */ 
		if (token[0] != 0x00D) {
			offset += 4;
		}
		token = strtok(NULL, delim);
	}
	
	addMnemonics(&symbols);
	
	for(iterator i = start(&symbols); i != end(&symbols); i = next(i)) {
		printf("%s -> %i\n", getKey(i), get(&symbols, getKey(i)));
	}

	freeTable(&symbols);
	
	fclose(in);
	free(buffer);

 	return EXIT_SUCCESS;
	
}
