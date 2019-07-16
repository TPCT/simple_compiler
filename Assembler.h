#include <stdio.h>
#include <malloc.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

typedef char *String;
typedef unsigned long long Counter;
typedef enum Instruction_Type {
    mov = 0, add, sub, mul, Div, cmp,
    jmp, jne, je, jge, jg, jle, jl, call, inc, dec,
    msg, end, ret, label, NULL_CODE = -1
} Instruction_Type;
typedef enum Variable_Type {
    INT, DOUBLE, STRING
} Variable_Type;
typedef struct Line *LinePtr;
typedef struct Register *RegPtr;
typedef struct Label *LabelPtr;
typedef struct Label {
    String LabelName;
    LinePtr Line_address;
    LabelPtr NexTLabel;
    Counter line_no;
} Label;
typedef struct Register {
    unsigned int register_number;
    String register_name;
    double register_data;
    RegPtr NextRegister;
} Register;
typedef struct Line {
    enum Instruction_Type instructionType;
    union {
        String Jump_Label;
        String MSG;
        LabelPtr lbl;
        struct operands {
            RegPtr register_a;
            union {
                double Operand_B;
                RegPtr register_B;
            } operand_b;
        } operands;
    } Data;
    LinePtr NextLine;
} Line;
typedef struct Error *ErrorPtr;
typedef struct Error {
    Counter Error_Count;
    Counter Error_Line;
    String ErrorMSG;
    ErrorPtr NexTError;
} Error;

static char *Instructions[] = {"MOV", "ADD", "SUB", "MUL", "DIV", "CMP", "JMP", "JNE",
                               "JE", "JGE", "JG", "JLE", "JL", "CALL", "INC", "DEC", "MSG", "END", "RET", NULL};

static LabelPtr lbl = NULL; // Label Tree
static RegPtr registers = NULL; //Registers Goes Here
static LinePtr Jumps = NULL;

LinePtr Codify(String);

LinePtr Tokenize(String);

LinePtr Combine_Jumps(LinePtr *, LinePtr, LabelPtr);

LabelPtr labelize(LabelPtr, String, LinePtr, LabelPtr *, Counter);

Instruction_Type Instruction_Identifier(String);

RegPtr Registerize(RegPtr, RegPtr *, String);

void operands_extractor(LinePtr, String *);

unsigned long long strtrim(String *);

void printLine(LinePtr);
