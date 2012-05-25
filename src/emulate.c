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
	uint32_t mask = 0xf8000000;
	/* Hex number for opCode mask */
	uint32_t res = inst & mask;
	return res >> 26;
}

uint32_t getR1(uint32_t inst) {
	uint32_t mask = 0x7C00000;
	/* Hex number for R1 mask */
	uint32_t res = inst & mask;
	return res >> 20;
}

uint32_t getR2(uint32_t inst) {
	uint32_t mask = 0x3E0000;
	/* Hex number for R2 mask */
	uint32_t res = inst & mask;
	return res >> 15;
}

uint32_t getR3(uint32_t inst) {
	uint32_t mask = 0x1F000;
	/* Hex number for R3 mask */
	uint32_t res = inst & mask;
	return res >> 10;
}

uint32_t getIVal(uint32_t inst) {
	uint32_t mask = 0x7FFF;
	/* Hex number for Immediate value mask */
	return inst & mask;
}

uint32_t getAddress(uint32_t inst) {
	uint32_t mask = 0x3FFFFFF;
	/* Hex number for Address mask */
	return inst & mask;
}

void processInstruction(uint32_t opCode, uint32_t inst, state_t st) {
	st.pc++;

	if (opCode == 15 || opCode == 17) {
		int addr = getAddress(inst);
		switch (opCode) {
			case 15:
				st.pc = addr;
				break;
			case 17:
				st.reg[31] = st.pc + 4;
				st.pc = addr;
				break;
				
			default: /* bla */;
		}
	} else if (opCode == 1 || opCode == 3 || opCode == 5 || opCode == 16 || opCode == 18) {
		int r1 = getR1(inst);
		int r2 = getR2(inst);
		int r3 = getR3(inst);
		switch (opCode) {
			case 1:
				st.reg[r1] = st.reg[r2] + st.reg[r3];
				break;
				
			case 3:
				st.reg[r1] = st.reg[r2] - st.reg[r3];
				break;
				
			case 5:
				st.reg[r1] = st.reg[r2] * st.reg[r3];
				break;
				
			case 16:
				st.pc = st.reg[r1];
				break;
				
			case 18:
				printf("Register 1: %i", st.reg[r1]);
				break;
				
			default: /* bla */;
		}
	} else {
		if (opCode == 0) {
			fprintf(stderr, "Program Counter: %i\n", st.pc);
			
			for(int i = 0; i < 32; i++) {
				fprintf(stderr, "Register %i: %i", i, st.reg[i]);
			}
		} else if (opCode <= 18) {
			int r1 = getR1(inst);
			int r2 = getR2(inst);
			int val = getR3(inst);
			switch (opCode) {
				case 2:
					st.reg[r1] = st.reg[r2] + val;
					break;
					
				case 4:
					st.reg[r1] = st.reg[r2] - val;
					break;
					
				case 6:
					st.reg[r1] = st.reg[r2] * val;
					break;
					
				case 7:
					st.reg[r1] = st.mem[r2 + val];
					break;
					
				case 8:
					st.mem[r2 + val] = st.reg[r1];
					break;
					
				case 9:
					if (st.reg[r1] == st.reg[r2]) {
						st.pc = st.pc + (val * 4);
					}
					break;
					
				case 10:
					if (st.reg[r1] != st.reg[r2]) {
						st.pc = st.pc + (val * 4);
					}
					break;
					
				case 11:
					if (st.reg[r1] < st.reg[r2]) {
						st.pc = st.pc + (val * 4);
					}
					break;
					
				case 12:
					if (st.reg[r1] > st.reg[r2]) {
						st.pc = st.pc + (val * 4);
					}
					break;
					
				case 13:
					if (st.reg[r1] <= st.reg[r2]) {
						st.pc = st.pc + (val * 4);
					}
					break;
					
				case 14:
					if (st.reg[r1] >= st.reg[r2]) {
						st.pc = st.pc + (val * 4);
					}
					break;
					
				default: /* bla */;
				
			}
		} else {
			/* Invalid opCode */
		}
	}
}

int main(int argc, char **argv) {
	state_t st;
	FILE *fp;
	uint32_t *ptr;

	st = init(st);
	
	if ((fp = fopen(argv[1], "rb")) == NULL) {
		perror("fopen");
		exit(EXIT_FAILURE);
	}

	fread(ptr, 4 * sizeof(char), 1, fp);
	printf("%i\n", *ptr);

	fclose(fp);
	free(st.mem);
 	return EXIT_SUCCESS;
}