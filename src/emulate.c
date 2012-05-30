 #include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include "emulate.h"

int main(int argc, char **argv) {
	FILE *fp, *fpres;
	int size;
	/* uint32_t *buffer; */

	state_t st = init();

	if ((fp = fopen(argv[1], "rb")) == NULL 
			|| (fpres = fopen(argv[2], "wb")) == NULL) {
		perror("fopen");
		exit(EXIT_FAILURE);
	}

	fseek(fp, 0, SEEK_END);
	size = ftell(fp) / 4;
	fseek(fp, 0, SEEK_SET);  /* size = number of 32-bit instructions in file */

	fread(st.mem, 4, size, fp);

	while (st.halt == 0) {
		st = executeInstruction(readUint32(st.pc, st), st, fpres);
	}

	fclose(fpres);
	fclose(fp);
	free(st.mem);

 	return EXIT_SUCCESS;
}
