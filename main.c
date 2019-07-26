#include "lexer.h"
#include "parser.h"
#include "codegenerator.h"
#include "options.h"

int main(int argc, char *args[]) {
    String Code = "MSG 'This program convert integer into binary repr\n'\n"
                  "MOV A, 10\n"
                  "MSG 'The value of A = ', A, '\n'\n"
                  "MOV B, 0\n"
                  "CALL bin_repr\n"
                  "MSG 'The Binary Repr Of A = ', B, '\n'\n"
                  "END\n"
                  "bin_repr:\n"
                  "CMP A, 0\n"
                  "MOV C, A\n"
                  "MOD C, 2\n"
                  "MUL B, 10\n"
                  "ADD B, C\n"
                  "DIV A, 2\n"
                  "JNE bin_repr\n"
                  "RET\n";
    codeReader(Code);
    parse();
    totalCompile();
}

