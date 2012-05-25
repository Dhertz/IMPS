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

uint32_t getOpCode(uint32_t inst) {
	uint32_t res;
	uint32_t mask = 15 << 27;
	
	res = inst & mask;
	return res >> 27;
}

int main(int argc, char **argv) {
	printf("%i\n", getOpCode(1164299827));
	

	/*
	struct state st;
	st = init(st);
	
	printf("%i\n", st.mem[0]);

	free(st.mem);
	*/
 	return EXIT_SUCCESS;
}