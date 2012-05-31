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

void init(struct table *l) {
	l->head = allocElem();
	l->foot = allocElem();
	l->head->prev = NULL;
	l->foot->next = NULL;
	l->head->next = l->foot;
	l->foot->prev = l->head;
}