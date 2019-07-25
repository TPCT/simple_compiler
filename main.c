#include "lexer.h"
#include "parser.h"
#include "codegenerator.h"

int main(int argc, char *args[]) {
    char Code[] =
            "MSG 'This Program Finds pow(5, 2)\n'\n"
            "END\n";
    codeReader(Code);
    parse();
    totalCompile();
}
