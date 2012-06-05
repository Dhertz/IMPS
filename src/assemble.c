#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <assert.h>
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

void writeInstruction(uint32_t opCode, uint32_t *data, FILE *fp) {
	assert (0 <= opCode && opCode <= 18);
	uint32_t inst = opCode << 26;

	if (opCode == 15 || opCode == 17) {
		/* J-type instructions */
		uint32_t addr = data[0];
		inst |= addr;
	} else if (opCode == 1 || opCode == 3 || opCode == 5 || opCode == 16 || opCode == 18) {
		/* R-type instructions */
		uint32_t r1 = data[0];
		uint32_t r2 = data[1];
		uint32_t r3 = data[2];
		inst |= (r1 << 21);
		inst |= (r2 << 16);
		inst |= (r3 << 11);
	} else if (opCode != 0) {
		/* I-type instructions */
		uint32_t r1 = data[0];
		uint32_t r2 = data[1];
		uint32_t val = data[2];
		inst |= (r1 << 21);
		inst |= (r2 << 16);
		inst |= val;
	}

	fwrite(&inst, 4, 1, fp);
}

int main(int argc, char **argv) {
	FILE *out;

	if ((out = fopen(argv[1], "wb")) == NULL) {
		perror("fopen");
		exit(EXIT_FAILURE);
	}

	/* Test input of beq $5 $8 24 */
	uint32_t opCode = 9;
	uint32_t *data = malloc(1 * sizeof(int));
	data[0] = 5;
	data[1] = 8;
	data[2] = 24;

	writeInstruction(opCode, data, out);

	free(data);
	fclose(out);
	return EXIT_SUCCESS;	
}
