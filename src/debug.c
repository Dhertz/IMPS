#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include "symtable.h"
#include "utils.h"

#define MAX_COMMAND_LENGTH 30

/*
 * TODO:
 *
 * 1. Write showHelp
 * 2. Command for printing PC? (What about setting?)
 */

void readCommand(char buffer[], int size) {
    fgets(buffer, size, stdin);
    
    int i = 0;
    while (buffer[i] != '\0') {
        if (buffer[i] == '\n') {
            buffer[i] = '\0';
        } else {
            i++;
        }
    }
}

static void _addCommand(table *t, char *longCmd, char *shortCmd, int pos) {
    insertFront(t, longCmd, pos);
    insertFront(t, shortCmd, pos);
}

void addCommands(table *t) {
    _addCommand(t, "quit", "q", 0);
    _addCommand(t, "break", "b", 1);
    _addCommand(t, "run", "r", 2);
    _addCommand(t, "next", "n", 3);
    _addCommand(t, "continue", "c", 4);
    _addCommand(t, "print", "p", 5);
    _addCommand(t, "help", "h", 6);
    _addCommand(t, "setReg", "sR", 7);
    _addCommand(t, "setAddr", "sA", 8);
    _addCommand(t, "printReg", "pR", 9);
    _addCommand(t, "printAddr", "pA", 10);
}

void showHelp() {
    printf("IMPS Debugger commands:\n\n");
    /* TODO: write these prints */
}

static bool _isEmptyLine(int line, int emptylinenos[], int emptyLines) {
    int i;
    for (i = 0; i < emptyLines; i++) {
        if (emptylinenos[i] == line) return true;
    }
    return false;
}

static int _mapLookup(int assLine, int lineNoMap[], int noLines) {
    for (int i = 0; i < noLines; i++) {
        if (lineNoMap[i] == assLine) return i;
    }
    return -1;
}

int main(int argc, char **argv) {
    FILE *in;
    long size;
    char *buffer, *origBuffer, *buffer2;

    printf("Reading input file %s... ", argv[1]);
    
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
    fclose(in);
    printf("done.\n");

    /* Create copies of buffer for use in second pass */
    buffer2 = malloc(size * 4);
    memcpy(buffer2, buffer, size);

    /* Detect empty lines, needed for human to assembly line number conversion in breakpoints */
    int emptylinenos[size];
    int emptylines = 0, curline = 0;
    bool prevEmpty = false;

    origBuffer = buffer;
    while (*buffer != '\0') {
        if (*buffer == '\n') {
            curline++;
            if (prevEmpty) {
                emptylinenos[emptylines] = curline;
                emptylines++;
            }
        }
        prevEmpty = *buffer == '\n';
        buffer++;
    }
    buffer = origBuffer;
     
    /* First pass - fill symbol table with labels -> offsets */
    printf("Building symbol table... ");
    table symbols;
    init(&symbols);
    int numLines = buildSymTable(&symbols, in, size, buffer);
    free(buffer);
    printf("done.\n");

    /* Second pass - create binary instructions */
    const char *delim = "\n";
    char *state;
    char *token = strtok_r(buffer2, delim, &state);
    state_t st = initState();
    
    /* Fill assembled with the binary instructions */
    int offset = 0;
    char **source = malloc(curline * sizeof(char*));
    if (source == NULL) {
        perror("malloc");
        exit(EXIT_FAILURE);
    }

    while (token != NULL) {
        /* Save token for printing at breakpoints */
        char *tokenCopy = malloc(strlen(token) * sizeof(char));
        strcpy(tokenCopy, token);
        source[offset] = tokenCopy;
        uint32_t inst = convertInstruction(token, symbols, offset, st);
        memcpy(st.mem + (offset * 4), &inst, sizeof(uint32_t));
        token = strtok_r(NULL, delim, &state);
        offset++;
    }

    /*
    START DEBUGGER
    */
    
    printf("Welcome to the IMPS debugger. Type \"h\" or \"help\" for information on commands.\n");
    
    /* Create symbol table to store synonomous commands */
    table commands;
    init(&commands);
    addCommands(&commands);
    
    /* Array to store line numbers of breakpoints */
    int* breaks = calloc(sizeof(int), numLines);
    int breakCount = 0;
    
    bool running = false;
    int line, lineCopy, regno, value, addr;

    /* Initialise line number map used in human to assembly line conversion */
    char *rest, *addrString, *end;
    int lineNoMap[curline + 1];
    for (int i = 0; i < curline + 1; i++) {
        lineNoMap[i] = i;
        for (int j = 0; j <= i; j++) {
            if (_isEmptyLine(j, emptylinenos, emptylines)) {
                lineNoMap[i]--;
            }
        }
    }

    /* Pre-run menu loop, for setting breakpoints */
    while (!running) {
        /* cmd = the whole input line */
        char cmd[MAX_COMMAND_LENGTH];
        readCommand(cmd, sizeof(cmd));
        
        /* cmdStr = the command keyword */
        char *cmdStr = strtok_r(cmd, " ", &rest);
        int cmdCode = get(&commands, cmdStr);
        
        switch (cmdCode) {
            case 0:
                /* quit - q */
                printf("Quitting...\n");
                goto endDebugger;
            case 1:
                /* break - b */
                line = atoi(strtok_r(NULL, " ", &rest));
                lineCopy = line;
                line = lineNoMap[line];
                breaks[breakCount] = line;
                breakCount++;
                printf("\nBreakpoint set at line %i.\n", lineCopy);
                break;
            case 2:
                /* run - r */
                running = true;
                break;
            case 6:
                /* help - h */
                showHelp();
                break;
            case 7:
                /* setRegister - sR */
                regno = atoi(strtok_r(NULL, " ", &rest));
                value = atoi(strtok_r(NULL, " ", &rest));
                st.reg[regno] = value;
                break;
            case 8:
                /* setAddress - sA */
                addrString = strtok_r(NULL, " ", &rest);
                addr = strtol(addrString, &end, 16);
                value = atoi(strtok_r(NULL, " ", &rest));
                st.mem[addr] = value;
                break;
            default:
                printf("Invalid command - %s.\n", cmdStr);
        }
    }
    
    /* START EMULATING */
    int linecount = 1;
    while (st.halt == 0) {
        for (int i = 0; i < breakCount; i++) {
            if (linecount == breaks[i]) {
                bool cont = false;
                
                printf("\nBreakpoint at line %i reached. What do you want to do?\n", _mapLookup(linecount, lineNoMap, curline + 1));
                
                while (!cont) {
                    printf("%i. %s\n", _mapLookup(linecount, lineNoMap, curline + 1), source[linecount - 1]);
                    
                    char cmd[MAX_COMMAND_LENGTH];
                    readCommand(cmd, sizeof(cmd));
                    char *rest;
                    char *cmdStr = strtok_r(cmd, " ", &rest);
                    int cmdCode = get(&commands, cmdStr);
                
                    switch (cmdCode) {
                        case 0:
                            /* quit - q */
                            printf("Quitting...\n");
                            goto endDebugger;
                        case 3:
                            /* next - n */
                            st = executeInstruction(readUint32(st.pc, st), st);
                            linecount++;
                            if (st.halt == 1) goto endDebugger;
                            break;
                        case 4:
                            /* continue - c */
                            cont = true;
                            break;
                        case 5:
                            /* print - p */
                            printReg(st);
                            break;
                        case 6:
                            /* help - h */
                            showHelp();
                            break;
                        case 7:
                            /* setRegister - sR */
                            regno = atoi(strtok_r(NULL, " ", &rest));
                            value = atoi(strtok_r(NULL, " ", &rest));
                            st.reg[regno] = value;
                            printf("$%i set to %i\n", regno, value);
                            break;
                        case 8:
                            /* setAddress - sA */
                            addrString = strtok_r(NULL, " ", &rest);
                            addr = strtol(addrString, &end, 16);
                            value = atoi(strtok_r(NULL, " ", &rest));
                            st.mem[addr] = value;
                            printf("mem[%i] set to %i\n", regno, value);
                            break;
                        case 9:
                            /* printRegister - pR */
                            regno = atoi(strtok_r(NULL, " ", &rest));
                            printf("$%i = %i\n", regno, st.reg[regno]);
                            break;
                        case 10:
                            /* printAddress - pA */
                            addrString = strtok_r(NULL, " ", &rest);
                            addr = strtol(addrString, &end, 16);
                            printf("mem[%i] = %i\n", addr, st.mem[addr]);
                            break;
                        default:
                            printf("Invalid command - %s.\n", cmdStr);
                    }
                }
            }    
        }
        st = executeInstruction(readUint32(st.pc, st), st);
        linecount++;
    }
    
    /*
    END DEBUGGER
    */
    endDebugger:
        for (int i = 0; i < offset; i++) {
            free(source[i]);
        }
        free(breaks);
        free(source);
        free(st.mem);
        freeTable(&symbols);
        free(buffer2);
        return EXIT_SUCCESS;
}
