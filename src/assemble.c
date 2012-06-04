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
						 "muli", "lw", "sw", "beq", "bne", "blt", "bgt",
						 "ble", "bge", "jmp", "jr", "jal", "out"};
								
	for(int i = 0; i <= 18; i++) {
		insertFront(t, mnemonics[i], i);
	}
}

int regConvert(char *reg) {
	reg++;
	int ret = atoi(reg);
	return ret;
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
	
	token = strtok(buffer, delim);
	
	while (token != NULL) {
		char *opcode = strtok_r(token, " ", &token);
		if (strchr(opcode, ':') != '\0') {
			opcode = strtok_r(NULL, " ", &token);
		}
		int mapped = get(&symbols, opcode);
		if(mapped == 1 || mapped == 3 || mapped == 5) {
			int vals[3];
			vals[0] = regConvert(strtok_r(NULL, " ", &token));
			vals[1] = regConvert(strtok_r(NULL, " ", &token));
			vals[2] = regConvert(strtok_r(NULL, " ", &token));
		} else if(mapped == 16 || mapped == 18) {
			int vals[1];
			vals[0] = regConvert(strtok_r(NULL, " ", &token));
		} else if(mapped == 15 || mapped == 17) {
			int vals[1];
			vals[0] = get(&symbols, strtok_r(NULL, " ", &token));
		} else {
			int vals[3];
			vals[0] = regConvert(strtok_r(NULL, " ", &token));
			vals[1] = regConvert(strtok_r(NULL, " ", &token));
			vals[2] = atoi(strtok_r(NULL, " ", &token));
		}
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
