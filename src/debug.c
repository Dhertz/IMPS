#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include "symtable.h"
#include "utils.h"

#define MAX_COMMAND_LENGTH 30

void readCommand(char buffer[], int size) {
    fgets(buffer, size, stdin);
    
    int letter = 0;
    while (buffer[letter] != '\0') {
        if (buffer[letter] == '\n') {
            buffer[letter] = '\0';
        } else {
            letter++;
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
     printf("quit (q): Halt the execution and exit the program\n");
     printf("break <line> (b <line>): Set a breakpoint at line <line>\n");
     printf("run (r): Start running the program\n");
     printf("continue (c): Continue execution to the next break point\n");
     printf("print (p): Print the contents of all registers\n");
     printf("help (h): Print this list of commands\n");
     printf("setReg <regno> <value> (sR <regno> <value>): Set the value of $<regno> to <value>\n");
	printf("setAddr <addr> <value> (sA <addr> <value>): Set the value of mem[<addr>] to <value>\n");
	printf("printReg <regno> (pR <regno>): Print the value of $<regno>\n");
	printf("printAddr <addr> (pA <addr>): Print the value of mem[<addr>]\n");
}

static bool _isEmptyLine(int line, int emptyLines[], int emptyLinesSize) {
    for (int i = 0; i < emptyLinesSize; i++) {
        if (emptyLines[i] == line) return true;
    }
    return false;
}

static int _mapLookup(int assLine, int lineNoMap[], int LineNoMapSize) {
    for (int i = 0; i < LineNoMapSize; i++) {
        if (lineNoMap[i] == assLine) return i;
    }
    return EXIT_FAILURE;
}

int main(int argc, char **argv) {
    FILE *in;
    long size;
    char *buffer, *bufferCopy1, *bufferCopy2;

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

    /* Detect empty lines, needed for human to assembly line number conversion in breakpoints */
    int emptyLineNos[size];
    int emptyLineNosSize = 0, curline = 0;
    bool prevEmpty = false;

    bufferCopy1 = buffer;
    while (*buffer != '\0') {
        if (*buffer == '\n') {
            curline++;
            if (prevEmpty) {
                emptyLineNos[emptyLineNosSize] = curline;
                emptyLineNosSize++;
            }
        }
        prevEmpty = *buffer == '\n';
        buffer++;
    }
    buffer = bufferCopy1;

    /* Create copies of buffer for use in second pass */
    bufferCopy2 = malloc(size * 4);
    memcpy(bufferCopy2, buffer, size);
    bufferCopy2[strlen(buffer)] = '\0';
    
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
    char *token = strtok_r(bufferCopy2, delim, &state);
    state_t st = initState();
    
    /* Fill assembled with the binary instructions */
    char **source = malloc(numLines * sizeof(char*));
    if (source == NULL) {
        perror("malloc");
        exit(EXIT_FAILURE);
    }
    curline = 0;
    while (token != NULL) {
        /* Save token for printing at breakpoints */
        char *tokenCopy = malloc(strlen(token) * sizeof(char));
        strcpy(tokenCopy, token);
        source[curline] = tokenCopy;
        uint32_t inst = convertInstruction(token, symbols, curline, st);
        memcpy(st.mem + (curline * 4), &inst, sizeof(uint32_t));
        token = strtok_r(NULL, delim, &state);
        curline++;
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
    int lineNoMap[numLines + 1];
    for (int i = 0; i < numLines + 1; i++) {
        lineNoMap[i] = i;
        for (int j = 0; j <= i; j++) {
            if (_isEmptyLine(j, emptyLineNos, emptyLineNosSize)) {
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
    curline = 1;
    while (st.halt == 0) {
        for (int i = 0; i < breakCount; i++) {
            if (curline == breaks[i]) {
                bool cont = false;
                
                printf("\nBreakpoint at line %i reached. What do you want to do?\n", _mapLookup(curline, lineNoMap, numLines + 1));
                
                while (!cont) {
                    printf("%i. %s\n", _mapLookup(curline, lineNoMap, numLines + 1), source[curline - 1]);
                    
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
                            curline++;
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
        curline++;
    }
    
    /*
    END DEBUGGER
    */
    endDebugger:
        for (int i = 0; i < numLines; i++) {
            free(source[i]);
        }
        free(breaks);
        free(source);
        free(st.mem);
        freeTable(&symbols);
        free(bufferCopy2);
        return EXIT_SUCCESS;
}
