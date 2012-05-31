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
void insertFront(table *t, char *k, int v);
int get(table *t, char *k);
void freeTable(table *t);

#endif
