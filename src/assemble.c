#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <assert.h>
#include "symtable.h"

char *getLabel(char *str) {
	char *st;
	strtok_r(str, ":", &st);
	char *res = str;
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

void writeInstruction(uint32_t opCode, uint32_t *data, FILE *fp) {
	assert (0 <= opCode && opCode <= 18);
	uint32_t inst = opCode << 26;

	if (opCode == 15 || opCode == 17) {
		/* J-type instructions */
		uint32_t addr = data[0];
		inst |= addr;
		printf("Opcode: %i\nAddress: %i\n\n", opCode, addr);
	} else if (opCode == 1 || opCode == 3 || opCode == 5 || opCode == 16 || opCode == 18) {
		/* R-type instructions */
		uint32_t r1 = data[0];
		uint32_t r2 = data[1];
		uint32_t r3 = data[2];
		inst |= (r1 << 21);
		inst |= (r2 << 16);
		inst |= (r3 << 11);
		printf("Opcode: %i\nR1: %i\nR2: %i\nR3: %i\n\n", opCode, r1, r2, r3);
	} else if (opCode != 0) {
		/* I-type instructions */
		uint32_t r1 = data[0];
		uint32_t r2 = data[1];
		uint32_t val = data[2];
		inst |= (r1 << 21);
		inst |= (r2 << 16);
		inst |= val;
		printf("Opcode: %i\nR1: %i\nR2: %i\nValue: %i\n\n", opCode, r1, r2, val);
	}

	fwrite(&inst, 4, 1, fp);
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
	char *state;
	char *token = strtok_r(buffer, delim, &state);
	int offset = 0;
	
	while (token != NULL) {
		if (strchr(token, ':') != '\0') {
			char *tmp = getLabel(token);
			insertFront(&symbols, tmp, offset);
		}
		/* 0x00D = Carriage return - might need more coverage here */ 
		if (token[0] != 0x00D) {
			offset += 4;
		}
		token = strtok_r(NULL, delim, &state);
	}
	
	addMnemonics(&symbols);
	
	free(buffer);
	
	fseek(in, 0, SEEK_END);
	size = ftell(in);
	fseek(in, 0, SEEK_SET);
	
	buffer = malloc(size * 4);
	fread(buffer, 4, size, in);
	
	FILE *out;
	if ((out = fopen(argv[2], "wb")) == NULL) {
		perror("fopen");
		exit(EXIT_FAILURE);
	}
	
	token = strtok_r(buffer, delim, &state);
	
	while (token != NULL) {
		printf("%s\n", token);
		char *tokstate;
		char *opcode = strtok_r(token, " ", &tokstate);
		if (strchr(opcode, ':') != '\0') {
			opcode = strtok_r(NULL, " ", &tokstate);
		}
		
		if (strcmp(opcode, ".fill") == 0) {
			
		} else if (strcmp(opcode, ".skip") == 0) {
		
		} else {
			uint32_t mapped = get(&symbols, opcode);
			if (mapped == 0) {
				uint32_t vals[1];
				vals[0] = mapped;
				writeInstruction(mapped, vals, out);
			} else if(mapped == 1 || mapped == 3 || mapped == 5) {
				uint32_t vals[3];
				vals[0] = regConvert(strtok_r(NULL, " ", &tokstate));
				vals[1] = regConvert(strtok_r(NULL, " ", &tokstate));
				vals[2] = regConvert(strtok_r(NULL, " ", &tokstate));
				writeInstruction(mapped, vals, out);
			} else if(mapped == 16 || mapped == 18) {
				uint32_t vals[3];
				vals[0] = regConvert(strtok_r(NULL, " ", &tokstate));
				vals[1] = 0;
				vals[2] = 0;
				writeInstruction(mapped, vals, out);
			} else if(mapped == 15 || mapped == 17) {
				uint32_t vals[1];
				vals[0] = get(&symbols, strtok_r(NULL, " ", &tokstate));
				writeInstruction(mapped, vals, out);
			} else {
				uint32_t vals[3];
				vals[0] = regConvert(strtok_r(NULL, " ", &tokstate));
				vals[1] = regConvert(strtok_r(NULL, " ", &tokstate));
				vals[2] = atoi(strtok_r(NULL, " ", &tokstate));
				writeInstruction(mapped, vals, out);
			}
		}
		
		token = strtok_r(NULL, delim, &state);
		if (token != NULL) {
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
