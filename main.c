#include "lexer.h"
#include "parser.h"
#include "codegenerator.h"

int main(int argc, char *args[]) {
    String Code;
    Code = "CALL func1\n"
           "MOV 5, 5\n"
           "JMP label\n"
           "MSG 'the value of register A: ', A\n"
           "END\n"
           "label:\n"
           "func1:\n"
           "CALL func2\n"
           "MOV A, 6\n"
           "RET\n"
           "func2:\n"
           "MOV A, 3\n"
           "RET\n";
    codeReader(Code);
    parse();
    //totalCompile();

}
