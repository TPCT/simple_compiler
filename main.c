#include "lexer.h"
#include "parser.h"
#include "codegenerator.h"

int main(int argc, char *args[]) {
    char Code[] = "MSG 'PLEASE READ GIT README'";
    codeReader(Code);
    parse();
    totalCompile();
}
