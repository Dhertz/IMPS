#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>

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
	FILE *fp;

	uint32_t x = 0;     /* really hacky, but defining just    */
	uint32_t *ptr = &x; /* uint32_t *ptr didn't work - ideas? */

	st = init(st);
	
	if ((fp = fopen(argv[1], "rb")) == NULL) {
		perror("fopen");
		exit(EXIT_FAILURE);
	}

	fread(ptr, 4 * sizeof(char), 1, fp);
	printf("%i\n", *ptr);

	fclose(fp);
	free(st.mem);
 	return EXIT_SUCCESS;
}