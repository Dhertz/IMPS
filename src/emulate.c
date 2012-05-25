#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>

typedef struct state {
	short *mem;
	short pc;
	int reg[32];
} state_t;

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

void processInstruction(uint32_t opCode, uint32_t inst, state_t st) {
	st.pc++;

	switch (opCode) {
		case 0:
			fprintf(stderr, "Program Counter: %i\n", st.pc);
			
			for(int i = 0; i < 32; i++) {
				fprintf(stderr, "Register %i: %i", i, st.reg[i]);
			}
			break;
		
		case 1:
			st.reg[getR1(inst)] = st.reg[getR2(inst)] + st.reg[getR3(inst)];
			break;
			
		case 2:
			st.reg[getR1(inst)] = st.reg[getR2(inst)] + getIVal(inst);
			break;
			
		case 3:
			st.reg[getR1(inst)] = st.reg[getR2(inst)] - st.reg[getR3(inst)];
			break;
			
		case 4:
			st.reg[getR1(inst)] = st.reg[getR2(inst)] - getIVal(inst);
			break;
			
		case 5:
			st.reg[getR1(inst)] = st.reg[getR2(inst)] * st.reg[getR3(inst)];
			break;
			
		case 6:
			st.reg[getR1(inst)] = st.reg[getR2(inst)] * getIVal(inst);
			break;
			
		case 7:
			st.reg[getR1(inst)] = st.mem[getR2(inst) + getIVal(inst)];
			break;
			
		case 8:
			st.mem[getR2(inst) + getIVal(inst)] = st.reg[getR1(inst)];
			break;
			
		case 9:
			if (st.reg[getR1(inst)] == st.reg[getR2(inst)]) {
				st.pc = st.pc + (getIVal(inst) * 4);
			}
			break;
			
		case 10:
			if (st.reg[getR1(inst)] != st.reg[getR2(inst)]) {
				st.pc = st.pc + (getIVal(inst) * 4);
			}
			break;
			
		case 11:
			if (st.reg[getR1(inst)] < st.reg[getR2(inst)]) {
				st.pc = st.pc + (getIVal(inst) * 4);
			}
			break;
			
		case 12:
			if (st.reg[getR1(inst)] > st.reg[getR2(inst)]) {
				st.pc = st.pc + (getIVal(inst) * 4);
			}
			break;
			
		case 13:
			if (st.reg[getR1(inst)] <= st.reg[getR2(inst)]) {
				st.pc = st.pc + (getIVal(inst) * 4);
			}
			break;
			
		case 14:
			if (st.reg[getR1(inst)] >= st.reg[getR2(inst)]) {
				st.pc = st.pc + (getIVal(inst) * 4);
			}
			break;
		
		case 15:
			st.pc = getAddress(inst);
			break;
			
		case 16:
			st.pc = st.reg[getR1(inst)];
			break;
			
		case 17:
			st.reg[31] = st.pc + 4;
			st.pc = getAddress(inst);
			break;
			
		case 18:
			printf("Register 1: %i", st.reg[getR1(inst)]);
			break;
		
		}
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