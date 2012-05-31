#include <stdlib.h>
#include <stdio.h>

struct table {
	node_t *head;
	node_t *foot;
};

typedef struct table_node {
	string key;
	int value;
	node_t *prev;
	node_t *next;
} node_t;

typedef node_t iterator;

void *allocElem(void) {
	node_t *new = malloc(sizeof(node_t));
	if (elem == NULL) {
		perror("allocElem");
		exit(EXIT_FAILURE);
	}
	return new;
}

void freeElem(node_t *elem) {
	free(elem);
}

void init(struct table *t) {
	t->head = allocElem();
	t->foot = allocElem();
	t->head->prev = NULL;
	t->foot->next = NULL;
	t->head->next = t->foot;
	t->foot->prev = t->head;
}

iterator start(struct table *t) {
	return t->head->next;
}

iterator end(struct table *t) {
	return t->foot;
}

void insert(struct table *t, iterator i, string key, int value) {
	node_t new = allocElem();
	new->key = key;
	new->value = value;
	
	new->prev = iter->prev;
	new->next = iter;
	iter->prev->next = new;
	iter->prev = new;
}