#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>

typedef struct state {
	uint16_t *mem;
	uint16_t pc;
	unint32_t reg[32];
}state_t;

state_t init(state_t st) {
	st.mem = calloc(4096, 16);

	if (st.mem == NULL) {
		perror("malloc");
		exit(EXIT_FAILURE);
	}

	return st;
}

uint32_t getOpCode(uint32_t inst) {
	uint32_t res;
	uint32_t mask = 31 << 26;

	res = inst & mask;
	return res >> 26;
}

int main(int argc, char **argv) {
	state_t st;
	st = init(st);

	printf("%i\n", st.mem[0]);

	free(st.mem);
 	return EXIT_SUCCESS;
}