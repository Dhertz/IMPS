#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include "debugger.h"

int main(int argc, char **argv) {
	FILE *fp;
	long size;

	state_t st = initState();

	if ((fp = fopen(argv[1], "rb")) == NULL) {
		perror("fopen");
		exit(EXIT_FAILURE);
	}

	fseek(fp, 0, SEEK_END);
	size = ftell(fp);
	fseek(fp, 0, SEEK_SET);  /* size = number of 32-bit instructions in file */
	
	fread(st.mem, 4, size, fp);

	while (st.halt == 0) {
		st = executeInstruction(readUint32(st.pc, st), st);
	}

	fclose(fp);
	free(st.mem);

 	return EXIT_SUCCESS;
}
