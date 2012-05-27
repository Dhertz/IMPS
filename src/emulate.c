#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>

typedef struct state {
	int8_t *mem;
 	uint16_t pc;
	int32_t reg[32];
	int halt;
} state_t;

typedef uint32_t inst_t;

state_t init(state_t st) {
	int i;

	st.mem = calloc(65536, 1);

	if (st.mem == NULL) {
		perror("malloc");
		exit(EXIT_FAILURE);
	}

	st.pc = 0;
	
	for (i = 0; i < 32; i ++) {
		st.reg[i] = 0;
	}

	st.halt = 0;

	return st;
}

uint8_t getOpCode(inst_t inst) {
	uint32_t mask = 0xFC000000;
	/* Hex number for opCode mask */
	uint32_t res = inst & mask;
	return res >> 26;
}

uint8_t getR1(inst_t inst) {
	uint32_t mask = 0x3E00000;
	/* Hex number for R1 mask */
	uint32_t res = inst & mask;
	return res >> 21;
}

uint8_t getR2(inst_t inst) {
	uint32_t mask = 0x1F0000;
	/* Hex number for R2 mask */
	uint32_t res = inst & mask;
	return res >> 16;
}

uint8_t getR3(inst_t inst) {
	uint32_t mask = 0x3E00;
	/* Hex number for R3 mask */
	uint32_t res = inst & mask;
	return res >> 11;
}

uint16_t getIVal(inst_t inst) {
	uint32_t mask = 0xFFFF;
	/* Hex number for Immediate value mask */
	return inst & mask;
}

uint16_t getAddress(inst_t inst) {
	uint32_t mask = 0x3FFFFFF;
	/* Hex number for Address mask */
	return inst & mask;
}

int32_t signExtension(int16_t val) {
	return (int32_t) val;
}

state_t executeInstruction(inst_t inst, state_t st, FILE *fpres) {
	uint8_t opCode = getOpCode(inst);
	st.pc += 4;

	/* fprintf(stderr, "Incremented PC: %i\n", st.pc); */
	fprintf(stderr, "Instruction: %i\n", inst);
	fprintf(stderr, "Opcode: %i\n\n", opCode);

	if (opCode == 15 || opCode == 17) {
		/* J-type instructions */
		uint16_t addr = getAddress(inst);
		switch (opCode) {
			case 15:
				/* jmp */
				/* fprintf(stderr, "Setting PC to %i\n", addr); */
				st.pc = addr;
				break;
			case 17:
				/* jal */
				st.reg[31] = st.pc + 1;
				st.pc = addr;
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
				fwrite(&st.reg[r1], 1, 1, fpres); /* stdout eventually, but this is easier for testing */
				break;
		}
	} else {
		if (opCode == 0) {
			/* Halt */
			fprintf(stderr, "PC: %i\n\n", st.pc);
			for(int i = 0; i < 32; i++) {
				fprintf(stderr, "R%i: %i\n", i, st.reg[i]);
			}
			st.halt = 1;
		} else if (opCode <= 18) {
			/* I-type instructions */
			uint8_t r1 = getR1(inst);
			uint8_t r2 = getR2(inst);
			int32_t val = signExtension(getIVal(inst));
			
			/* fprintf(stderr, "Instruction: %i\n", inst);
			fprintf(stderr, "Opcode: %i\n", opCode);
			fprintf(stderr, "R1: %i\n", r1);
			fprintf(stderr, "R2: %i\n", r2);
			fprintf(stderr, "C: %i\n\n", val); */

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
						st.pc = st.pc + (val * 4);
					}
					break;
				case 10:
					/* bne */
					if (st.reg[r1] != st.reg[r2]) {
						st.pc = st.pc + (val * 4);
					}
					break;
				case 11:
					/* blt */
					if (st.reg[r1] < st.reg[r2]) {
						st.pc = st.pc + (val * 4);
					}
					break;
				case 12:
					/* bgt */
					if (st.reg[r1] > st.reg[r2]) {
						st.pc = st.pc + (val * 4);
					}
					break;
				case 13:
					/* ble */
					if (st.reg[r1] <= st.reg[r2]) {
						st.pc = st.pc + (val * 4);
					}
					break;
				case 14:
					/* bge */
					if (st.reg[r1] >= st.reg[r2]) {
						st.pc = st.pc + (val * 4);
					}
					break;
				default:
					fprintf(stderr, "Invalid opcode");
					break;
			}
		} else {
			fprintf(stderr, "Invalid opcode");
		}
	}
	
	return st;
}

uint32_t readUint32(int addr, state_t st) {
	uint32_t res = 0;
	int i;

	for (i = 0; i < 4; i++) {
		res |= st.mem[addr + i] << (8 * i);
	}

	return res;
}

int main(int argc, char **argv) {
	state_t st;
	FILE *fp, *fpres;
	int size;
	/* uint32_t *buffer; */

	st = init(st);

	if ((fp = fopen(argv[1], "rb")) == NULL) {
		perror("fopen");
		exit(EXIT_FAILURE);
	}

	if ((fpres = fopen(argv[2], "wb")) == NULL) {
		perror("fopen");
		exit(EXIT_FAILURE);
	}

	fseek(fp, 0, SEEK_END);
	size = ftell(fp) / 4;
	fseek(fp, 0, SEEK_SET);  /* size = number of 32-bit instructions in file */

	/* buffer = malloc(size * 4);
	fread(buffer, 4, size, fp); */
	fread(st.mem, 4, size, fp);

	for (int i = 0; i < 400; i += 4)
		printf("st.mem[%i] = %i\n", i, readUint32(i, st));
	

	while (st.halt == 0) {
		/* st = executeInstruction(buffer[st.pc], st, fpres); */
		st = executeInstruction(readUint32(st.pc, st), st, fpres);
	}

	fclose(fpres);
	fclose(fp);
	free(st.mem);

 	return EXIT_SUCCESS;
}
