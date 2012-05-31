#include <stdlib.h>
#include <stdio.h>
#include <string.h>

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

void *allocElem(void) {
	node_t *new = malloc(sizeof(node_t));
	if (new == NULL) {
		perror("allocElem");
		exit(EXIT_FAILURE);
	}
	return new;
}

void freeElem(node_t *elem) {
	free(elem);
}

void init(table *t) {
	t->head = allocElem();
	t->foot = allocElem();
	t->head->prev = NULL;
	t->foot->next = NULL;
	t->head->next = t->foot;
	t->foot->prev = t->head;
}

iterator start(table *t) {
	return t->head->next;
}

iterator next(iterator i) {
	return i->next;
}

char *getKey(iterator i) {
	return i->key;
}

void insert(table *t, iterator i, char *k, int v) {
	node_t *new = allocElem();
	new->key = k;
	new->value = v;
	
	new->prev = i->prev;
	new->next = i;
	i->prev->next = new;
	i->prev = new;
}

void insertFront(table *t, char *k, int v) {
	insert(t, start(t), k, v);
}

int get(table *t, char *k) {
	iterator i = start(t);
	while(i != NULL) {
		if(strcmp(getKey(i), k) == 0) {
			return i->value;
		}
	}
	return (int) NULL; /* is this right? not sure... */
}

void freeTable(table *t) {
	node_t *elem = t->head;
	while(elem != NULL) {
		node_t *next = elem->next;
		freeElem(elem);
		elem = next;
	}
}