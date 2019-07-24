#include "lexer.h"
#include "parser.h"
#include "codegenerator.h"

int main(int argc, char *args[]) {
    String Code;
    Code = "MSG 'This Program Finds pow(5, 2)\n'\n"
           "MOV A, 5\n"
           "MOV B, 2\n"
           "INC A\n"
           "DEC A\n"
           "MSG A\n"
           "LABEL:\n"
           "JMP LABEL\n"
           "CMP A, B\n"
           "JLE LABEL\n"
           "END\n";
    codeReader(Code);
    //parse();
    //totalCompile();
}
