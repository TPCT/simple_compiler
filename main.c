#include "lexer.h"
#include "parser.h"
#include "codegenerator.h"

int main(int argc, char *args[]) {
    String Code;
    Code = "MOV A, 5\n"
           "MOV B, A\n"
           "MSG 'A = ', A, '\n'\n"
           "MSG 'B = ', B, '\n'\n"
           "END\n";
    codeReader(Code);
    parse();
    totalCompile();
}
