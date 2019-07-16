#include "Assembler.h"

int main() {
    char code[] = "DEC X\nINC X\nMOV X, Y\nJMP lbl\nlbl:\nMOV N, Y\nTPCT:TEMP:";
    LinePtr tokens = Codify(code);
    while (tokens) {
        printLine(tokens);
        tokens = tokens->NextLine;
    }
    free(tokens);
    return 0;
}