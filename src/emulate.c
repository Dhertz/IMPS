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
	int i;
	uint32_t *buffer = malloc(12 * 4);

	st = init(st);
	
	if ((fp = fopen(argv[1], "rb")) == NULL) {
		perror("fopen");
		exit(EXIT_FAILURE);
	}

	fread(buffer, 4, 12, fp);

	for (i = 0; i < 12; i++) {
		printf("%i\n", buffer[i]);
	}

	fclose(fp);
	free(st.mem);
 	return EXIT_SUCCESS;
}