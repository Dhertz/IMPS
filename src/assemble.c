#include <stdlib.h>
#include <stdio.h>
#include "symtable.h"

int main(int argc, char **argv) {
	table t;
	init(&t);
	
	insertFront(&t, "hello", 3);
	insertFront(&t, "hi", 4);
	
	for(iterator i = start(&t); i != end(&t); i = next(i)) {
		printf("%s -> %i\n", getKey(i), get(&t, getKey(i)));
	}
	
	freeTable(&t);
	return 0;
}
