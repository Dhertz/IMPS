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
	int i, size;
	uint32_t *buffer;

	st = init(st);

	if ((fp = fopen(argv[1], "rb")) == NULL) {
		perror("fopen");
		exit(EXIT_FAILURE);
	}

	fseek(fp, 0, SEEK_END);
	size = ftell(fp) / 4;
	fseek(fp, 0, SEEK_SET); /* size = number of 32-bit instructions in file */

	buffer = malloc(size * 4);
	fread(buffer, 4, size, fp);

	for (i = 0; i < size; i++) {
		printf("%i\n", buffer[i]);
	}

	fclose(fp);
	free(st.mem);
 	return EXIT_SUCCESS;
}