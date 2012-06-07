#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <assert.h>
#include "symtable.h"

char *getLabel(char *str) {
    char *st;
    strtok_r(str, ":", &st);
    char *res = str;
    return res;
}

void addMnemonics(table *t) {
    char *mnemonics[] = {"halt", "add", "addi", "sub", "subi", "mul",
                         "muli", "lw", "sw", "beq", "bne", "blt", "bgt",
                         "ble", "bge", "jmp", "jr", "jal", "out"};
                                
    for(int i = 0; i <= 18; i++) {
        insertFront(t, mnemonics[i], i);
    }
}

void writeInstruction(uint32_t opCode, uint32_t *data, FILE *fp) {
    assert (0 <= opCode && opCode <= 18);
    uint32_t inst = opCode << 26;

    if (opCode == 15 || opCode == 17) {
        /* J-type instructions */
        uint32_t addr = data[0];
        inst |= addr;
    } else if (opCode == 1 || opCode == 3 || opCode == 5 || opCode == 16 || 
               opCode == 18) {
        /* R-type instructions */
        uint32_t r1 = data[0];
        uint32_t r2 = data[1];
        uint32_t r3 = data[2];
        inst |= (r1 << 21);
        inst |= (r2 << 16);
        inst |= (r3 << 11);
    } else if (opCode != 0) {
        /* I-type instructions */
        uint32_t r1 = data[0];
        uint32_t r2 = data[1];
        int32_t val = data[2];
        inst |= (r1 << 21);
        inst |= (r2 << 16);
        inst |= (val & 0xFFFF);
    }

    fwrite(&inst, 4, 1, fp);
}

int regConvert(char *reg) {
    reg++;
    int ret = atoi(reg);
    return ret;
}

int main(int argc, char **argv) {
    FILE *in;
    long size;
    char *buffer;
    char *buffer2;

    /* Open and read from input file */
    if ((in = fopen(argv[1], "rb")) == NULL) {
        perror("fopen");
        exit(EXIT_FAILURE);
    }
    
    fseek(in, 0, SEEK_END);
    size = ftell(in);
    fseek(in, 0, SEEK_SET);
    
    buffer = malloc(size * 4);
    fread(buffer, 4, size, in);
    
    /* Create copy of buffer for use in second pass */
    buffer2 = malloc(size * 4);
    memcpy(buffer2, buffer, size);
    
    /* Create symbol table */
    table symbols;
    init(&symbols);
    
    /* First pass - fill symbol table with labels -> offsets */
    const char *delim = "\n";
    char *state;
    char *token = strtok_r(buffer, delim, &state);
    int offset = 0;
    
    while (token != NULL) {
        if (strchr(token, ':') != '\0') {
            char *tmp = getLabel(token);
            insertFront(&symbols, tmp, offset);
        }
        /* 0x00D = Carriage return - ignore empty lines */ 
        if (token[0] != 0x00D) {
            offset += 4;
        }
        token = strtok_r(NULL, delim, &state);
    }
    
    /* Add Mnemonics -> Opcodes to symbol table */
    addMnemonics(&symbols);
    free(buffer);
    
    /* Set up output file */
    FILE *out;
    if ((out = fopen(argv[2], "wb")) == NULL) {
        perror("fopen");
        exit(EXIT_FAILURE);
    }
    
    /* Second pass - create binary instructions */
    token = strtok_r(buffer2, delim, &state);
    offset = 0;
    
    while (token != NULL) {
        char *tokstate;
        char *opcode = strtok_r(token, " ", &tokstate);
        
        /* Jump over the label if there is one */
        if (strchr(opcode, ':') != '\0') {
            opcode = strtok_r(NULL, " ", &tokstate);
        }
        
        if (strcmp(opcode, ".fill") == 0) {
            uint32_t val = atoi(strtok_r(NULL, " ", &tokstate));
            fwrite(&val, 4, 1, out);
        } else if (strcmp(opcode, ".skip") == 0) {
            int size = atoi(strtok_r(NULL, " ", &tokstate));
            uint32_t x[size];
            memset(x, 0, size*4);
            fwrite(&x, 4, size, out);
        } else {
            /* mapped = opCode number */
            uint32_t mapped = get(&symbols, opcode);
            
            /* Create an array of isntruction parameters, based on the
               instruction type and opCode, which can then be passed to
               writeInstruction to be written to the output file */
            if (mapped == 0) {
                /* halt */
                uint32_t vals[1];
                vals[0] = mapped;
                writeInstruction(mapped, vals, out);
            } else if (mapped == 1 || mapped == 3 || mapped == 5) {
                /* 3 register R-type instructions */
                uint32_t vals[3];
                vals[0] = regConvert(strtok_r(NULL, " ", &tokstate));
                vals[1] = regConvert(strtok_r(NULL, " ", &tokstate));
                vals[2] = regConvert(strtok_r(NULL, " ", &tokstate));
                writeInstruction(mapped, vals, out);
            } else if (mapped == 16 || mapped == 18) {
                /* J-type instructions */
                uint32_t vals[3];
                vals[0] = regConvert(strtok_r(NULL, " ", &tokstate));
                vals[1] = 0;
                vals[2] = 0;
                writeInstruction(mapped, vals, out);
            } else if (mapped == 15 || mapped == 17) {
                /* 1 register R-type instructions */
                uint32_t vals[1];
                vals[0] = get(&symbols, strtok_r(NULL, " ", &tokstate));
                writeInstruction(mapped, vals, out);
            } else {
                /* I-type instructions */
                uint32_t vals[3];
                vals[0] = regConvert(strtok_r(NULL, " ", &tokstate));
                vals[1] = regConvert(strtok_r(NULL, " ", &tokstate));
                char *val = strtok_r(NULL, " ", &tokstate);
                
                /* Check if the first character is a letter or not */
                if (val[0] < 0x0041) {
                    /* Check if the numerical value is hex */
                    if (strchr(val, 'x') != '\0') {
                        char *end;
                        vals[2] = strtol(val, &end, 16);
                    } else {
                        vals[2] = atoi(val);
                    }
                } else {
                    /* Divide C by four if the instruction requires it */
                    if (mapped == 2 || mapped == 4 || mapped == 6 || 
                        mapped == 7 || mapped == 8) {
                        vals[2] = get(&symbols, val);
                    } else {
                        vals[2] = (get(&symbols, val) / 4) - offset;
                    }
                }
                writeInstruction(mapped, vals, out);
            }
        }
        
        token = strtok_r(NULL, delim, &state);
        offset++;
        if (token != NULL) {
            /* Jump over lines with just newlines (or other ctrl characters) */
            if (strcmp(token, "\r") <= 0) {
                token = strtok_r(NULL, delim, &state);
            }
        }
    }
    
    freeTable(&symbols);
    
    fclose(in);
    fclose(out);
    free(buffer2);
    return EXIT_SUCCESS;    
}
