#include "Assembler.h"

int main() {
    char code[] = "label0:label1:label2:";
    LinePtr tokens = Codify(code);
    while (tokens) {
        printLine(*tokens);
        tokens = tokens->NextLine;
    }
    return 0;
}