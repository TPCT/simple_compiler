#include "lexer.h"
#include "parser.h"
#include "codegenerator.h"

int main(int argc, char *args[]) {
    char Code[] =
            "MSG 'This Program Finds pow(5, 2)\n'\n"
            "MOV A, 5\n"
            "CALL LABEL\n"
            "END\n"
            "LABEL :\n"
            "MSG 'THIS MSG IS FROM FUNCTION LABEL\n'\n"
            "RET\n";
    codeReader(Code);
    parse();
    totalCompile();
}
