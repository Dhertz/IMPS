#ifndef DEBUGGER_H
#define DEBUGGER_H

typedef struct table_node {
	char *key;
	uint32_t value;
	struct table_node *prev;
	struct table_node *next;
} node_t;

typedef struct table {
	node_t *head;
	node_t *foot;
} table;

typedef node_t *iterator;

typedef struct state {
	uint8_t *mem;
 	uint16_t pc;
	int32_t reg[32];
	int halt;
} state_t;

typedef uint32_t inst_t;

char *getLabel(char *str);
void addMnemonics(table *t);
table buildSymTable(FILE *in, long size, char* buffer);
uint32_t convertInstruction(char *token, table symbols, int offset);
state_t initState(void);
state_t executeInstruction(inst_t inst, state_t st);
inst_t readUint32(int addr, state_t st);
void init(table *t);
iterator start(table *t);
iterator end(table *t);
iterator next(iterator i);
char *getKey(iterator i);
void insertFront(table *t, char *k, int v);
uint32_t get(table *t, char *k);
void freeTable(table *t);
char *strtok_r(char *s1, const char *s2, char **s3);

#endif
