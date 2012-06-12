#ifndef DEBUGGER_H
#define DEBUGGER_H

void readCommand(char buffer[], int size);
void addCommands(table *t);
void showHelp();

#endif
