#include "Assembler.h"

char *Instructions[] = {"MOV", "INC", "DEC", "ADD", "SUB", "MUL", "DIV", "CMP", "JMP", "JNE", "JNE",
                        "JE", "JGE", "JG", "JLE", "JL", "CALL", "MSG", "END", NULL};
LabelPtr lbl = NULL;

LinePtr Codify(String Code) {
    String line_code = strsep(&Code, "\n");
    LinePtr line = NULL;
    do {
        line = Tokenize(line_code);
    } while (Code);
    return line;
}

LinePtr Tokenize(String Line_Code) {
    static char max_recursion = 0;
    if (max_recursion == 100) {
        max_recursion = 0;
        fprintf(stderr, "%s", "Too many recursions\n");
        return NULL;
    }
    max_recursion += 1;
    if (*Line_Code == ';' || !Line_Code) return NULL;
    LinePtr line = NULL;
    String Token = strsep(&Line_Code, " ");
    unsigned long long Line_Length = strtrim(&Token);
    if (*(Token + Line_Length - 1) == ':') {
        Line_Code = strdup(Token);
        line = (LinePtr) malloc(sizeof(Line));
        Token = strsep(&Line_Code, ":"),
                strtrim(&Token),
        line->instructionType = label,
        line->Settings = 0b10000000;
        labelize(lbl, Token, line, &(line->Data.lbl)),
                line->NextLine = Tokenize(Line_Code);
    }
    return line;
}

LabelPtr labelize(LabelPtr labl, String lblName, LinePtr lblAddress, LabelPtr *SavingLabel) {
    if (!labl)
        labl = (LabelPtr) malloc(sizeof(Label)),
        labl->LabelName = lblName,
        labl->Line_address = lblAddress,
        labl->NexTLabel = NULL,
                *SavingLabel = labl;
    else if (labl->LabelName == lblName)
        return labl;
    else
        labl->NexTLabel = labelize(labl->NexTLabel, lblName, lblAddress, SavingLabel);
    return labl;
}

Instruction_Type Instruction_Identifier(String instruction) {
    for (char i = 0; Instructions[i]; i++)
        if (!strcmp(Instructions[i], instruction))
            return i;
    return NULL_CODE;
}

void printLine(Line line) {
    printf("===================START LINE==================\n");
    switch (line.instructionType) {
        case label:
            printf("Instruction Type = label\n");
            printf("Settings = %d\n", line.Settings);
            printf("Data\nMSG = (nil)\nlbl = %p\nlbl name = %s\nlbl Address %p\nOperands = (nil)\n",
                   line.Data.lbl, line.Data.lbl->LabelName, line.Data.lbl->Line_address);
            printf("NextLine = %p\n", line.NextLine);
            break;
    }
}

unsigned long long strtrim(String *str) {
    while (**str == ' ') (*str)++;
    unsigned long long i = 0;
    for (; *(*str + i); i++);
    while (*(*str + i - 1) == ' ')
        *(*str + i-- - 1) = '\0';
    return i;
}
