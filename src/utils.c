#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <assert.h>
#include "symtable.h"
#include "utils.h"

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
                                
    for (int i = 0; i <= 18; i++) {
        insertFront(t, mnemonics[i], i);
    }
}

int buildSymTable(table *symbols, FILE *in, long size, char *buffer) {
    /* First pass - fill symbol table with labels -> offsets */
    const char *delim = "\n";
    char *state;
    char *token = strtok_r(buffer, delim, &state);
    int offset = 0;
	int numLines = 0;
    
    while (token != NULL) {
		numLines++;
        if (strchr(token, ':') != '\0') {
            char *tmp = getLabel(token);
            insertFront(symbols, tmp, offset);
        }
        /* 0x00D = Carriage return - ignore empty lines */ 
        if (token[0] != 0x00D) {
            offset += 4;
        }
        token = strtok_r(NULL, delim, &state);
    }
    
    /* Add Mnemonics -> Opcodes to symbol table */
    addMnemonics(symbols);
	
	return numLines;
}

static uint32_t writeInstruction(uint32_t opCode, uint32_t *data) {
    assert (0 <= opCode && opCode <= 18);
    uint32_t inst = opCode << 26;

    if (opCode == 15 || opCode == 17) {
        /* J-type instructions */
        uint32_t addr = data[0];
        inst |= addr;
    } else if (opCode == 1 || opCode == 3 || opCode == 5 || opCode == 16 || 
               opCode == 18) {
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
        int32_t val = data[2];
        inst |= (r1 << 21);
        inst |= (r2 << 16);
        inst |= (val & 0xFFFF);
    }

    return inst;
}

static int regConvert(char *reg) {
    reg++;
    int ret = atoi(reg);
    return ret;
}

uint32_t convertInstruction(char *token, table symbols, int offset, state_t st) {
	char *tokstate;
    char *opcode = strtok_r(token, " ", &tokstate);
    
    /* Jump over the label if there is one */
    if (strchr(opcode, ':') != '\0') {
        opcode = strtok_r(NULL, " ", &tokstate);
    }
    
    if (strcmp(opcode, ".fill") == 0) {
        uint32_t val = atoi(strtok_r(NULL, " ", &tokstate));
        return val;
    } else if (strcmp(opcode, ".skip") == 0) {
		/* Set memory to 0 here (apart from first memory location which is 
		   done on return) */
		int size = atoi(strtok_r(NULL, " ", &tokstate));
		
		for (int i = 1; i < size; i++) {
			memset(st.mem + ((offset * 4) + (4 * i)), 0, sizeof(uint32_t));
		}
		return 0;
    } else {
        /* mapped = opCode number */
        uint32_t mapped = get(&symbols, opcode);
        
        /* Create an array of isntruction parameters, based on the
           instruction type and opCode, which can then be passed to
           writeInstruction to be written to the output file */
        if (mapped == 0) {
            /* halt */
            uint32_t vals[1];
            vals[0] = mapped;
            return writeInstruction(mapped, vals);
        } else if (mapped == 1 || mapped == 3 || mapped == 5) {
            /* 3 register R-type instructions */
            uint32_t vals[3];
            vals[0] = regConvert(strtok_r(NULL, " ", &tokstate));
            vals[1] = regConvert(strtok_r(NULL, " ", &tokstate));
            vals[2] = regConvert(strtok_r(NULL, " ", &tokstate));
            return writeInstruction(mapped, vals);
        } else if (mapped == 16 || mapped == 18) {
            /* J-type instructions */
            uint32_t vals[3];
            vals[0] = regConvert(strtok_r(NULL, " ", &tokstate));
            vals[1] = 0;
            vals[2] = 0;
            return writeInstruction(mapped, vals);
        } else if (mapped == 15 || mapped == 17) {
            /* 1 register R-type instructions */
            uint32_t vals[1];
            vals[0] = get(&symbols, strtok_r(NULL, " ", &tokstate));
            return writeInstruction(mapped, vals);
        } else {
            /* I-type instructions */
            uint32_t vals[3];
            vals[0] = regConvert(strtok_r(NULL, " ", &tokstate));
            vals[1] = regConvert(strtok_r(NULL, " ", &tokstate));
            char *val = strtok_r(NULL, " ", &tokstate);
            
            /* Check if the first character is a letter or not */
            if (val[0] < 0x0041) {
                /* Check if the numerical value is hex */
                if (strchr(val, 'x') != '\0') {
                    char *end;
                    vals[2] = strtol(val, &end, 16);
                } else {
                    vals[2] = atoi(val);
                }
            } else {
                /* Divide C by four if the instruction requires it */
                if (mapped == 2 || mapped == 4 || mapped == 6 || 
                    mapped == 7 || mapped == 8) {
                    vals[2] = get(&symbols, val);
                } else {
                    vals[2] = (get(&symbols, val) / 4) - offset;
                }
            }
            return writeInstruction(mapped, vals);
        }
    }
	return 0; /* Halt for unrecognised command. This should never be hit anyway. */
}

static uint8_t getOpCode(inst_t inst) {
	uint32_t mask = 0xFC000000;
	/* Hex number for opCode mask */
	uint32_t res = inst & mask;
	return res >> 26;
}

static uint8_t getR1(inst_t inst) {
	uint32_t mask = 0x3E00000;
	/* Hex number for R1 mask */
	uint32_t res = inst & mask;
	return res >> 21;
}

static uint8_t getR2(inst_t inst) {
	uint32_t mask = 0x1F0000;
	/* Hex number for R2 mask */
	uint32_t res = inst & mask;
	return res >> 16;
}

static uint8_t getR3(inst_t inst) {
	uint32_t mask = 0x7C00;
	/* Hex number for R3 mask */
	uint32_t res = inst & mask;
	return res >> 11;
}

static uint16_t getIVal(inst_t inst) {
	uint32_t mask = 0xFFFF;
	/* Hex number for Immediate value mask */
	return inst & mask;
}

static uint16_t getAddress(inst_t inst) {
	uint32_t mask = 0x3FFFFFF;
	/* Hex number for Address mask */
	return inst & mask;
}

static int32_t signExtension(int16_t val) {
	return (int32_t) val;
}

state_t executeInstruction(inst_t inst, state_t st) {
	uint8_t opCode = getOpCode(inst);
	st.pc += 4;
    
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
				st.reg[31] = st.pc;
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
				fprintf(stdout, "%c", st.reg[r1]);
				break;
		}
	} else {
		if (opCode == 0) {
			/* Halt */
			printReg(st);
			st.halt = 1;
			// This isn't right, means appropriate free commands wont be executed:
			exit(0);
		} else if (opCode <= 18) {
			/* I-type instructions */
			uint8_t r1 = getR1(inst);
			uint8_t r2 = getR2(inst);
			int32_t val = signExtension(getIVal(inst));
			
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
						st.pc = st.pc - 4 + (val * 4);				
					}
					break;
				case 10:
					/* bne */
					if (st.reg[r1] != st.reg[r2]) {
						st.pc = st.pc - 4 + (val * 4);
					}
					break;
				case 11:
					/* blt */
					if (st.reg[r1] < st.reg[r2]) {
						st.pc = st.pc - 4 + (val * 4);
					}
					break;
				case 12:
					/* bgt */
					if (st.reg[r1] > st.reg[r2]) {
						st.pc = st.pc - 4 + (val * 4);
					}
					break;
				case 13:
					/* ble */
					if (st.reg[r1] <= st.reg[r2]) {
						st.pc = st.pc - 4 + (val * 4);
					}
					break;
				case 14:
					/* bge */
					if (st.reg[r1] >= st.reg[r2]) {
						st.pc = st.pc - 4 + (val * 4);
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

void printReg(state_t st) {
	fprintf(stderr, "PC: %i\n\n", st.pc);
	for (int i = 0; i < 32; i++) {
		fprintf(stderr, "R%i: %i\n", i, st.reg[i]);
	}
}

inst_t readUint32(int addr, state_t st) {
	inst_t res = 0;
	int i;
    
	for (i = 0; i < 4; i++) {
		res |= st.mem[addr + i] << (8 * i);
	}
    
	return res;
}

state_t initState(void) {
	state_t st = {.pc = 0, .halt = 0, .reg = {0}};
	
	st.mem = calloc(65536, 1);
    
	if (st.mem == NULL) {
		perror("malloc");
		exit(EXIT_FAILURE);
	}
    
	return st;
}
