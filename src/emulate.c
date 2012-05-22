#include <stdlib.h>
#include <stdio.h>
#include <string.h>

struct state {
	short *mem;
	short pc;
	int reg[32];
};

struct state init(struct state st) {
	st.mem = calloc(4096, sizeof(short));

	if (st.mem == NULL) {
		perror("malloc");
		exit(EXIT_FAILURE);
	}

	return st;
}

int main(int argc, char **argv) {
	struct state st;
	st = init(st);
	
	printf("%i\n", st.mem[0]);

	free(st.mem);
 	return EXIT_SUCCESS;
}