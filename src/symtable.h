#ifndef SYMTABLE_H
#define SYMTABLE_H

typedef struct table_node {
	char *key;
	int value;
	struct table_node *prev;
	struct table_node *next;
} node_t;

typedef struct table {
	node_t *head;
	node_t *foot;
} table;

typedef node_t *iterator;

void init(table *t);
iterator start(table *t);
iterator end(table *t);
iterator next(iterator i);
char *getKey(iterator i);
void insertFront(table *t, char *k, int v);
int get(table *t, char *k);
void freeTable(table *t);

#endif
