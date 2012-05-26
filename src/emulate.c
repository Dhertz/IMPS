#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>

typedef struct state {
	uint8_t *mem;
 	uint16_t pc;
	uint32_t reg[32];
} state_t;

state_t init(state_t st) {
	st.mem = calloc(65536, 1);

	if (st.mem == NULL) {
		perror("malloc");
		exit(EXIT_FAILURE);
	}

	return st;
}

uint8_t getOpCode(uint32_t inst) {
	uint32_t mask = 0xf8000000;
	/* Hex number for opCode mask */
	uint32_t res = inst & mask;
	return res >> 26;
}

uint8_t getR1(uint32_t inst) {
	uint32_t mask = 0x3E00000;
	/* Hex number for R1 mask */
	uint32_t res = inst & mask;
	return res >> 20;
}

uint8_t getR2(uint32_t inst) {
	uint32_t mask = 0x1F0000;
	/* Hex number for R2 mask */
	uint32_t res = inst & mask;
	return res >> 15;
}

uint8_t getR3(uint32_t inst) {
	uint32_t mask = 0x3E00;
	/* Hex number for R3 mask */
	uint32_t res = inst & mask;
	return res >> 10;
}

uint16_t getIVal(uint32_t inst) {
	uint32_t mask = 0xFFFF;
	/* Hex number for Immediate value mask */
	return inst & mask;
}

uint16_t getAddress(uint32_t inst) {
	uint32_t mask = 0x3FFFFFF;
	/* Hex number for Address mask */
	return inst & mask;
}

int32_t signExtension(int16_t val) {
	return (int32_t)val;
}

state_t executeInstruction(uint32_t inst, state_t st) {
	uint8_t opCode = getOpCode(inst);
	st.pc++;

	if (opCode == 15 || opCode == 17) {
		/* J-type instructions */
		uint16_t addr = getAddress(inst);
		switch (opCode) {
			case 15:
				/* jmp */
				st.pc = addr;
				break;
			case 17:
				/* jal */
				st.reg[31] = st.pc + 1; /* instruction says +4, but buffer is addressable in */
				st.pc = addr;           /* 4-byte blocks, so +1 is fine                      */
				break;
		}
	} else if (opCode == 1 || opCode == 3 || opCode == 5 || opCode == 16 || opCode == 18) {
		/* R-type instructions */
		uint8_t r1 = getR1(inst);
		uint8_t r2 = getR2(inst);
		uint8_t r3 = getR3(inst);
		switch (opCode) {
			case 1:
				/* add */
				st.reg[r1] = st.reg[r2] + st.reg[r3];
				break;
			case 3:
				/* sub */
				st.reg[r1] = st.reg[r2] - st.reg[r3];
				break;
			case 5:
				/* mul */
				st.reg[r1] = st.reg[r2] * st.reg[r3];
				break;
			case 16:
				/* jr */
				st.pc = st.reg[r1];
				break;
			case 18:
				/* out */
				printf("%i", st.reg[r1]); /* needs to stdout in binary mode */
				break;
		}
	} else {
		if (opCode == 0) {
			/* Halt */
			fprintf(stderr, "PC: %i\n\n", st.pc);
			for(int i = 0; i < 32; i++) {
				fprintf(stderr, "R%i: %i", i, st.reg[i]);
			}
		} else if (opCode <= 18) {
			/* I-type instructions */
			uint8_t r1 = getR1(inst);
			uint8_t r2 = getR2(inst);
			uint16_t val = getIVal(inst);
			switch (opCode) {
				case 2:
					/* addi */
					st.reg[r1] = st.reg[r2] + val;
					break;
				case 4:
					/* subi */
					st.reg[r1] = st.reg[r2] - val;
					break;
				case 6:
					/* muli */
					st.reg[r1] = st.reg[r2] * val;
					break;
				case 7:
					/* lw */
					st.reg[r1] = st.mem[st.reg[r2] + val];
					break;
				case 8:
					/* sw */
					st.mem[st.reg[r2] + val] = st.reg[r1];
					break;
				case 9:
					/* beq */
					if (st.reg[r1] == st.reg[r2]) {
						st.pc = st.pc + val;
					}
					break;
				case 10:
					/* bne */
					if (st.reg[r1] != st.reg[r2]) {
						st.pc = st.pc + val;
					}
					break;
				case 11:
					/* blt */
					if (st.reg[r1] < st.reg[r2]) {
						st.pc = st.pc + val;
					}
					break;
				case 12:
					/* bgt */
					if (st.reg[r1] > st.reg[r2]) {
						st.pc = st.pc + val;
					}
					break;
				case 13:
					/* ble */
					if (st.reg[r1] <= st.reg[r2]) {
						st.pc = st.pc + val;
					}
					break;
				case 14:
					/* bge */
					if (st.reg[r1] >= st.reg[r2]) {
						st.pc = st.pc + val;
					}
					break;
				default:
					/* Invalid opCode */
					break;
			}
		} else {
			/* Invalid opCode */
		}
	}
	
	return st;
}

int main(int argc, char **argv) {
	state_t st;
	FILE *fp;
	int i, size;
	uint32_t *buffer;

	st = init(st);

	if ((fp = fopen(argv[1], "rb")) == NULL) {
		perror("fopen");
		exit(EXIT_FAILURE);
	}

	fseek(fp, 0, SEEK_END);
	size = ftell(fp) / 4;
	fseek(fp, 0, SEEK_SET);  /* size = number of 32-bit instructions in file */

	buffer = malloc(size * 4);
	fread(buffer, 4, size, fp);

	for (i = 0; i < size; i++) {
		printf("%i\n", buffer[i]);
	}

	fclose(fp);
	free(st.mem);
 	return EXIT_SUCCESS;

}