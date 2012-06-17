#ifndef UTILS_H
#define UTILS_H

typedef struct state {
	uint8_t *mem;
 	uint16_t pc;
	int32_t reg[32];
	int halt;
} state_t;

typedef uint32_t inst_t;

char *getLabel(char *str);
void addMnemonics(table *t);
int buildSymTable(table *symbols, FILE *in, long size, char* buffer);
uint32_t convertInstruction(char *token, table symbols, int offset, state_t st);
state_t executeInstruction(inst_t inst, state_t st);
void printReg(state_t st);
inst_t readUint32(int addr, state_t st);
state_t initState(void);

#endif
