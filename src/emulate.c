#include <stdlib.h>
#include <stdio.h>
#include <string.h>

typedef struct state {
	short *mem;
	short pc;
	int reg[32];
}state_t;

state_t init(state_t st) {
	st.mem = calloc(4096, sizeof(short));

	if (st.mem == NULL) {
		perror("malloc");
		exit(EXIT_FAILURE);
	}

	return st;
}

int main(int argc, char **argv) {
	state_t st;
	st = init(st);

	printf("%i\n", st.mem[0]);

	free(st.mem);
 	return EXIT_SUCCESS;
}