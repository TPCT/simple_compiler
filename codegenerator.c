#include "codegenerator.h"
#include "tokens.h"

#define GPT (*line)->generalPurposeTokenPtr->Tokens
void totalCompile(void) {
    Line_Ptr linePtr = Actual_Lines_tree;
    String msgData;
    if (!Error_Counter) {
        while (linePtr) {
            /*if (linePtr->linetype == callLine
            || linePtr->linetype == jmpLine)
                printLine(linePtr);
                This line is to print the calling and jumping arguments*/
            callCompile(&linePtr);
            retCompile(&linePtr);
            cmpCompiler(linePtr);
            jmpCompile(&linePtr);
            aluCompile(linePtr);
            msgData = extractMsg(linePtr);
            //printLine(linePtr); /* this line to print any other argument*/
            if (msgData)
                printf("%s", msgData);
            free(msgData);
            if (linePtr->linetype == endLine)
                exit(0);
            linePtr = linePtr->Next_Line;
        }
    } else {
        printAllStackCode(linePtr);
    }
    printf("\n%s\n", "This program exited with an error");
    exit(-1);
}

void aluCompile(Line_Ptr line) {
    if (line->linetype == aluLine) {
        GENERAL_PURPOSE_TOKEN_PTR GPT_PTR = line->generalPurposeTokenPtr;
        Alu_Token_Ptr aluTokenPtr = line->generalPurposeTokenPtr->Tokens.Alu_Token;
        switch (GPT_PTR->Tokens.Alu_Token->Instruction) {
            case MOV:
                if (aluTokenPtr->RegisterB->Data_Type_Settings)
                    aluTokenPtr->RegisterA->Data_Type_Settings = 1,
                            aluTokenPtr->RegisterA->Register_Values.Dval = aluTokenPtr->RegisterB->Register_Values.Dval;
                else
                    aluTokenPtr->RegisterA->Data_Type_Settings = 0,
                            aluTokenPtr->RegisterA->Register_Values.Lval = aluTokenPtr->RegisterB->Register_Values.Lval;
                break;
            case MUL:
                if (aluTokenPtr->RegisterB->Data_Type_Settings && aluTokenPtr->RegisterA->Data_Type_Settings)
                    aluTokenPtr->RegisterA->Data_Type_Settings = 1,
                            aluTokenPtr->RegisterA->Register_Values.Dval *= aluTokenPtr->RegisterB->Register_Values.Dval;
                else if (aluTokenPtr->RegisterB->Data_Type_Settings && !aluTokenPtr->RegisterA->Data_Type_Settings)
                    aluTokenPtr->RegisterA->Data_Type_Settings = 1,
                    aluTokenPtr->RegisterA->Register_Values.Dval = (double) aluTokenPtr->RegisterA->Register_Values.Lval,
                    aluTokenPtr->RegisterA->Register_Values.Dval *= aluTokenPtr->RegisterB->Register_Values.Dval;
                else if (!aluTokenPtr->RegisterB->Data_Type_Settings && !aluTokenPtr->RegisterA->Data_Type_Settings)
                    aluTokenPtr->RegisterA->Data_Type_Settings = 0,
                            aluTokenPtr->RegisterA->Register_Values.Lval *= aluTokenPtr->RegisterB->Register_Values.Lval;
                else
                    aluTokenPtr->RegisterA->Data_Type_Settings = 1,
                            aluTokenPtr->RegisterA->Register_Values.Dval *= (double) aluTokenPtr->RegisterB->Register_Values.Lval;
                break;
            case DIV:
                if (aluTokenPtr->RegisterB->Data_Type_Settings && aluTokenPtr->RegisterA->Data_Type_Settings)
                    aluTokenPtr->RegisterA->Data_Type_Settings = 1,
                            aluTokenPtr->RegisterA->Register_Values.Dval /= aluTokenPtr->RegisterB->Register_Values.Dval;
                else if (aluTokenPtr->RegisterB->Data_Type_Settings && !aluTokenPtr->RegisterA->Data_Type_Settings)
                    aluTokenPtr->RegisterA->Data_Type_Settings = 1,
                    aluTokenPtr->RegisterA->Register_Values.Dval = (double) aluTokenPtr->RegisterA->Register_Values.Lval,
                    aluTokenPtr->RegisterA->Register_Values.Dval /= aluTokenPtr->RegisterB->Register_Values.Dval;
                else if (!aluTokenPtr->RegisterB->Data_Type_Settings && !aluTokenPtr->RegisterA->Data_Type_Settings)
                    aluTokenPtr->RegisterA->Data_Type_Settings = 0,
                            aluTokenPtr->RegisterA->Register_Values.Lval /= aluTokenPtr->RegisterB->Register_Values.Lval;
                else
                    aluTokenPtr->RegisterA->Data_Type_Settings = 1,
                            aluTokenPtr->RegisterA->Register_Values.Dval /= (double) aluTokenPtr->RegisterB->Register_Values.Lval;
                break;
            case MOD:
                aluTokenPtr->RegisterA->Register_Values.Lval %= aluTokenPtr->RegisterB->Register_Values.Lval;
                break;
            case INC:
                if (aluTokenPtr->RegisterA->Data_Type_Settings)
                    aluTokenPtr->RegisterA->Register_Values.Dval += 1;
                else
                    aluTokenPtr->RegisterA->Data_Type_Settings = 0,
                            aluTokenPtr->RegisterA->Register_Values.Lval += 1;
                break;
            case DEC:
                if (aluTokenPtr->RegisterA->Data_Type_Settings)
                    aluTokenPtr->RegisterA->Register_Values.Dval -= 1;
                else
                    aluTokenPtr->RegisterA->Register_Values.Lval -= 1;
                break;
            case ADD:
                if (aluTokenPtr->RegisterB->Data_Type_Settings && aluTokenPtr->RegisterA->Data_Type_Settings)
                    aluTokenPtr->RegisterA->Data_Type_Settings = 1,
                            aluTokenPtr->RegisterA->Register_Values.Dval += aluTokenPtr->RegisterB->Register_Values.Dval;
                else if (aluTokenPtr->RegisterB->Data_Type_Settings && !aluTokenPtr->RegisterA->Data_Type_Settings)
                    aluTokenPtr->RegisterA->Data_Type_Settings = 1,
                    aluTokenPtr->RegisterA->Register_Values.Dval = (double) aluTokenPtr->RegisterA->Register_Values.Lval,
                    aluTokenPtr->RegisterA->Register_Values.Dval += aluTokenPtr->RegisterB->Register_Values.Dval;
                else if (!aluTokenPtr->RegisterB->Data_Type_Settings && !aluTokenPtr->RegisterA->Data_Type_Settings)
                    aluTokenPtr->RegisterA->Data_Type_Settings = 0,
                            aluTokenPtr->RegisterA->Register_Values.Lval += aluTokenPtr->RegisterB->Register_Values.Lval;
                else
                    aluTokenPtr->RegisterA->Data_Type_Settings = 1,
                            aluTokenPtr->RegisterA->Register_Values.Dval += (double) aluTokenPtr->RegisterB->Register_Values.Lval;
                break;
            case SUB:
                if (aluTokenPtr->RegisterB->Data_Type_Settings && aluTokenPtr->RegisterA->Data_Type_Settings)
                    aluTokenPtr->RegisterA->Data_Type_Settings = 1,
                            aluTokenPtr->RegisterA->Register_Values.Dval -= aluTokenPtr->RegisterB->Register_Values.Dval;
                else if (aluTokenPtr->RegisterB->Data_Type_Settings && !aluTokenPtr->RegisterA->Data_Type_Settings)
                    aluTokenPtr->RegisterA->Data_Type_Settings = 1,
                    aluTokenPtr->RegisterA->Register_Values.Dval = (double) aluTokenPtr->RegisterA->Register_Values.Lval,
                    aluTokenPtr->RegisterA->Register_Values.Dval -= aluTokenPtr->RegisterB->Register_Values.Dval;
                else if (!aluTokenPtr->RegisterB->Data_Type_Settings && !aluTokenPtr->RegisterA->Data_Type_Settings)
                    aluTokenPtr->RegisterA->Data_Type_Settings = 0,
                            aluTokenPtr->RegisterA->Register_Values.Lval -= aluTokenPtr->RegisterB->Register_Values.Lval;
                else
                    aluTokenPtr->RegisterA->Data_Type_Settings = 1,
                            aluTokenPtr->RegisterA->Register_Values.Dval -= (double) aluTokenPtr->RegisterB->Register_Values.Lval;
                break;
            case AND:
                aluTokenPtr->RegisterA->Register_Values.Lval = (unsigned) aluTokenPtr->RegisterA->Register_Values.Lval &
                                                               (unsigned) aluTokenPtr->RegisterB->Register_Values.Lval;
                break;
            case OR:
                aluTokenPtr->RegisterA->Register_Values.Lval = (unsigned) aluTokenPtr->RegisterA->Register_Values.Lval |
                                                               (unsigned) aluTokenPtr->RegisterB->Register_Values.Lval;
                break;
            case NOT:
                aluTokenPtr->RegisterA->Register_Values.Lval = ~(unsigned) aluTokenPtr->RegisterA->Register_Values.Lval;
                break;
        }
    }
}

void cmpCompiler(Line_Ptr line) {
    if (line->linetype == cmpLine) {
        if (line->generalPurposeTokenPtr->Tokens.CMP_Token->RegisterA
            && line->generalPurposeTokenPtr->Tokens.CMP_Token->RegisterB) {
            double registerAValue = (double) (line->generalPurposeTokenPtr->Tokens.CMP_Token->RegisterA->Data_Type_Settings
                                              ?
                                              line->generalPurposeTokenPtr->Tokens.CMP_Token->RegisterA->Register_Values.Dval
                                              :
                                              (double) line->generalPurposeTokenPtr->Tokens.CMP_Token->RegisterA->Register_Values.Lval);
            double registerBValue = (double) (line->generalPurposeTokenPtr->Tokens.CMP_Token->RegisterB->Data_Type_Settings
                                              ?
                                              line->generalPurposeTokenPtr->Tokens.CMP_Token->RegisterB->Register_Values.Dval
                                              :
                                              (double) line->generalPurposeTokenPtr->Tokens.CMP_Token->RegisterB->Register_Values.Lval);
            if (registerAValue == registerBValue)
                line->generalPurposeTokenPtr->Tokens.CMP_Token->CMP_Val = EQUAL;
            else if (registerAValue > registerBValue)
                line->generalPurposeTokenPtr->Tokens.CMP_Token->CMP_Val = GREATER_THAN;
            else
                line->generalPurposeTokenPtr->Tokens.CMP_Token->CMP_Val = LESS_THAN;
        }
    }
}

void jmpCompile(Line_Ptr *line) {
    if ((*line)->linetype == jmpLine) {
        switch (GPT.Jump_Token->Instruction) {
            case JMP:
                *line = GPT.Jump_Token->Line_Address;
                break;
            case JE:
                if (GPT.Jump_Token->CMP_Token->CMP_Val == EQUAL)
                    *line = GPT.Jump_Token->Line_Address;
                break;
            case JGE:
                if (GPT.Jump_Token->CMP_Token->CMP_Val == GREATER_THAN
                    || GPT.Jump_Token->CMP_Token->CMP_Val == EQUAL)
                    *line = GPT.Jump_Token->Line_Address;
                break;
            case JLE:
                if (GPT.Jump_Token->CMP_Token->CMP_Val == LESS_THAN
                    || GPT.Jump_Token->CMP_Token->CMP_Val == EQUAL)
                    *line = GPT.Jump_Token->Line_Address;
                break;
            case JNE:
                if (GPT.Jump_Token->CMP_Token->CMP_Val != EQUAL)
                    *line = GPT.Jump_Token->Line_Address;
                break;
            case JG:
                if (GPT.Jump_Token->CMP_Token->CMP_Val == GREATER_THAN)
                    *line = GPT.Jump_Token->Line_Address;
                break;
            case JL:
                if (GPT.Jump_Token->CMP_Token->CMP_Val == LESS_THAN)
                    *line = GPT.Jump_Token->Line_Address;
                break;
        }
    }
}

void callCompile(Line_Ptr *line) {
    if ((*line)->linetype == callLine)
        *GPT.Call_Token->callLabel = True,
        *GPT.Call_Token->callLine = *line,
        (*line) = GPT.Call_Token->labelLine;
}

void retCompile(Line_Ptr *line) {
    if ((*line)->linetype == returnLine
        && GPT.returnToken->called) {
        GPT.returnToken->called = False;
        *line = GPT.returnToken->last_caller;
    }
}

/** TODO:
 * include codes from other files
 * adding std libs -> for specific operations
 * adding handling strings in registers
 * nested functions handling
 * user input handling
**/
