#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>

struct state {
	short *mem;
	short pc;
	int reg[32];
};

struct state init(struct state st) {
	st.mem = calloc(4096, sizeof(short));

	if (st.mem == NULL) {
		perror("malloc");
		exit(EXIT_FAILURE);
	}

	return st;
}

uint32_t getOpCode(uint32_t inst) {
	uint32_t res;
	uint32_t mask = 0xf8000000;
	/* Hex number for opCode mask */
	res = inst & mask;
	return res >> 26;
}

uint32_t getR1(uint32_t inst) {
	uint32_t res;
	uint32_t mask = 0x7C00000;
	/* Hex number for R1 mask */
	res = inst & mask;
	return res >> 20;
}

uint32_t getR2(uint32_t inst) {
	uint32_t res;
	uint32_t mask = 0x3E0000;
	/* Hex number for R2 mask */
	res = inst & mask;
	return res >> 15;
}

uint32_t getR3(uint32_t inst) {
	uint32_t res;
	uint32_t mask = 0x1F000;
	/* Hex number for R3 mask */
	res = inst & mask;
	return res >> 10;
}

uint32_t getIVal(uint32_t inst) {
	uint32_t res;
	uint32_t mask = 0x7FFF;
	/* Hex number for Immediate value mask */
	res = inst & mask;
	return res;
}

uint32_t getAddress(uint32_t inst) {
	uint32_t res;
	uint32_t mask = 0x3FFFFFF;
	/* Hex number for Address mask */
	res = inst & mask;
	return res;
}

void processInstruction(uint32_t opCode, uint32_t) {
	
}

int main(int argc, char **argv) {
	FILE *in;
	char *buffer;
	int size;
	const char *path = *(argv + 1);
	
	if ((in = fopen(path, "r")) == NULL) {
		perror(path);
		exit(EXIT_FAILURE);
	}
	
	fseek(in, 0, SEEK_END);
	size = ftell(in);
	fseek(in, 0, SEEK_SET);
	
	buffer = malloc(size*sizeof(char));
	
	if(fgets(buffer, sizeof(buffer), in) == NULL) {
		perror("file read failed");
		exit(EXIT_FAILURE);
	}
	fclose(in);
	
	printf("%c", *(buffer+1));
	
	/*
	struct state st;
	st = init(st);
	
	printf("%i\n", st.mem[0]);

	free(st.mem);
	*/
	
	free(buffer);
 	return EXIT_SUCCESS;
}