#include "lexer.h"
#include "parser.h"
#include "codegenerator.h"

int main(int argc, char *args[]) {
    char Code[] =
            "MSG 'THIS PROGRAM CALCULATES 7!\n'\n"
            "MOV A, 7\n"
            "MOV B, 1\n"
            "CALL fact_7\n"
            "MSG '7! = ', B, '\n'\n"
            "END\n"
            "fact_7 :\n"
            "MUL B, A\n"
            "DEC A\n"
            "CMP A, 0\n"
            "JG fact_7\n"
            "RET\n";
    codeReader(Code);
    parse();
    totalCompile();
}
