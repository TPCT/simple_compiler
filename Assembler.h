#include <stdio.h>
#include <malloc.h>
#include <string.h>

typedef char *String;
typedef enum Instruction_Type {
    mov = 0, inc, dec, add, sub, mul, div, cmp,
    jmp = 8, jne, je, jge, jg, jle, jl,
    call = 15, msg, end, label, NULL_CODE = -1
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
} Label;
typedef struct Register {
    unsigned int register_number;
    String register_name;
    double register_data;
    RegPtr NextRegister;
} Register;
typedef struct Line {
    enum Instruction_Type instructionType;
    unsigned char Settings;
    union {
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

LinePtr Codify(String);

LinePtr Tokenize(String);

LabelPtr labelize(LabelPtr, String, LinePtr, LabelPtr *);

unsigned long long strtrim(String *);

void printLine(Line);
