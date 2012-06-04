#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "symtable.h"

static void *allocElem(void) {
	node_t *new = malloc(sizeof(node_t));
	if (new == NULL) {
		perror("allocElem");
		exit(EXIT_FAILURE);
	}
	return new;
}

static void freeElem(node_t *elem) {
	free(elem);
	free(elem->k);
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

iterator end(table *t) {
	return t->foot;
}

iterator next(iterator i) {
	return i->next;
}

char *getKey(iterator i) {
	return i->key;
}

static void insert(table *t, iterator i, char *k, int v) {
	printf("%s\n", k);
	node_t *new = allocElem();
	malloc(sizeof(char) * strlen(k));
	strcopy(new->key, k);
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
	while (i != NULL) {
		if (strcmp(getKey(i), k) == 0) {
			return i->value;
		} else {
			i = next(i);
		}
	}
	return -1;
}

void freeTable(table *t) {
	node_t *elem = t->head;
	while (elem != NULL) {
		node_t *next = elem->next;
		freeElem(elem);
		elem = next;
	}
}
