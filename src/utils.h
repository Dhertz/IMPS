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
uint32_t writeInstruction(uint32_t opCode, uint32_t *data);
int regConvert(char *reg);
uint32_t convertInstruction(char *token, table symbols, int offset);
uint8_t getOpCode(inst_t inst);
uint8_t getR1(inst_t inst);
uint8_t getR2(inst_t inst);
uint8_t getR3(inst_t inst);
uint16_t getIVal(inst_t inst);
uint16_t getAddress(inst_t inst);
int32_t signExtension(int16_t val);
state_t executeInstruction(inst_t inst, state_t st);
inst_t readUint32(int addr, state_t st);
state_t initState(void);

#endif