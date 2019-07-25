#include "codegenerator.h"
#include "tokens.h"

#define GPT (*line)->generalPurposeTokenPtr->Tokens
void totalCompile(void) {
    Line_Ptr linePtr = Actual_Lines_tree;
    String msgData;
    if (!Error_Counter) {
        while (linePtr) {
            /*if (linePtr->generalPurposeTokenPtr->tokenType == CALL_TOKEN_TYPE
            || linePtr->generalPurposeTokenPtr->tokenType == JUMP_TOKEN_TYPE)
                printLine(linePtr);
                This line is to print the calling and jumping arguments*/
            callCompile(&linePtr);
            retCompile(&linePtr);
            cmpCompiler(linePtr);
            jmpCompile(&linePtr);
            auCompile(linePtr);
            msgData = extractMsg(linePtr);
            /* this line to print any other argument printLine(linePtr);*/
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
}

void auCompile(Line_Ptr line) {
    if (line->linetype == auLine) {
        GENERAL_PURPOSE_TOKEN_PTR GPT_PTR = line->generalPurposeTokenPtr;
        Au_Token_Ptr auTokenPtr = line->generalPurposeTokenPtr->Tokens.Au_Token;
        switch (GPT_PTR->Tokens.Au_Token->Instruction) {
            case MOV:
                if (auTokenPtr->RegisterB->Data_Type_Settings)
                    auTokenPtr->RegisterA->Data_Type_Settings = 1,
                            auTokenPtr->RegisterA->Register_Values.Dval = auTokenPtr->RegisterB->Register_Values.Dval;
                else
                    auTokenPtr->RegisterA->Data_Type_Settings = 0,
                            auTokenPtr->RegisterA->Register_Values.Lval = auTokenPtr->RegisterB->Register_Values.Lval;
                break;
            case ADD:
                if (auTokenPtr->RegisterB->Data_Type_Settings && auTokenPtr->RegisterA->Data_Type_Settings)
                    auTokenPtr->RegisterA->Data_Type_Settings = 1,
                            auTokenPtr->RegisterA->Register_Values.Dval += auTokenPtr->RegisterB->Register_Values.Dval;
                else if (auTokenPtr->RegisterB->Data_Type_Settings && !auTokenPtr->RegisterA->Data_Type_Settings)
                    auTokenPtr->RegisterA->Data_Type_Settings = 1,
                    auTokenPtr->RegisterA->Register_Values.Dval = auTokenPtr->RegisterA->Register_Values.Lval,
                    auTokenPtr->RegisterA->Register_Values.Dval += auTokenPtr->RegisterB->Register_Values.Dval;
                else if (!auTokenPtr->RegisterB->Data_Type_Settings && !auTokenPtr->RegisterA->Data_Type_Settings)
                    auTokenPtr->RegisterA->Data_Type_Settings = 0,
                            auTokenPtr->RegisterA->Register_Values.Lval += auTokenPtr->RegisterB->Register_Values.Lval;
                else
                    auTokenPtr->RegisterA->Data_Type_Settings = 1,
                            auTokenPtr->RegisterA->Register_Values.Dval += auTokenPtr->RegisterB->Register_Values.Lval;
                break;
            case SUB:
                if (auTokenPtr->RegisterB->Data_Type_Settings && auTokenPtr->RegisterA->Data_Type_Settings)
                    auTokenPtr->RegisterA->Data_Type_Settings = 1,
                            auTokenPtr->RegisterA->Register_Values.Dval -= auTokenPtr->RegisterB->Register_Values.Dval;
                else if (auTokenPtr->RegisterB->Data_Type_Settings && !auTokenPtr->RegisterA->Data_Type_Settings)
                    auTokenPtr->RegisterA->Data_Type_Settings = 1,
                    auTokenPtr->RegisterA->Register_Values.Dval = auTokenPtr->RegisterA->Register_Values.Lval,
                    auTokenPtr->RegisterA->Register_Values.Dval -= auTokenPtr->RegisterB->Register_Values.Dval;
                else if (!auTokenPtr->RegisterB->Data_Type_Settings && !auTokenPtr->RegisterA->Data_Type_Settings)
                    auTokenPtr->RegisterA->Data_Type_Settings = 0,
                            auTokenPtr->RegisterA->Register_Values.Lval -= auTokenPtr->RegisterB->Register_Values.Lval;
                else
                    auTokenPtr->RegisterA->Data_Type_Settings = 1,
                            auTokenPtr->RegisterA->Register_Values.Dval -= auTokenPtr->RegisterB->Register_Values.Lval;
                break;
            case MUL:
                if (auTokenPtr->RegisterB->Data_Type_Settings && auTokenPtr->RegisterA->Data_Type_Settings)
                    auTokenPtr->RegisterA->Data_Type_Settings = 1,
                            auTokenPtr->RegisterA->Register_Values.Dval *= auTokenPtr->RegisterB->Register_Values.Dval;
                else if (auTokenPtr->RegisterB->Data_Type_Settings && !auTokenPtr->RegisterA->Data_Type_Settings)
                    auTokenPtr->RegisterA->Data_Type_Settings = 1,
                    auTokenPtr->RegisterA->Register_Values.Dval = auTokenPtr->RegisterA->Register_Values.Lval,
                    auTokenPtr->RegisterA->Register_Values.Dval *= auTokenPtr->RegisterB->Register_Values.Dval;
                else if (!auTokenPtr->RegisterB->Data_Type_Settings && !auTokenPtr->RegisterA->Data_Type_Settings)
                    auTokenPtr->RegisterA->Data_Type_Settings = 0,
                            auTokenPtr->RegisterA->Register_Values.Lval *= auTokenPtr->RegisterB->Register_Values.Lval;
                else
                    auTokenPtr->RegisterA->Data_Type_Settings = 1,
                            auTokenPtr->RegisterA->Register_Values.Dval *= auTokenPtr->RegisterB->Register_Values.Lval;
                break;
            case DIV:
                if (auTokenPtr->RegisterB->Data_Type_Settings && auTokenPtr->RegisterA->Data_Type_Settings)
                    auTokenPtr->RegisterA->Data_Type_Settings = 1,
                            auTokenPtr->RegisterA->Register_Values.Dval /= auTokenPtr->RegisterB->Register_Values.Dval;
                else if (auTokenPtr->RegisterB->Data_Type_Settings && !auTokenPtr->RegisterA->Data_Type_Settings)
                    auTokenPtr->RegisterA->Data_Type_Settings = 1,
                    auTokenPtr->RegisterA->Register_Values.Dval = auTokenPtr->RegisterA->Register_Values.Lval,
                    auTokenPtr->RegisterA->Register_Values.Dval /= auTokenPtr->RegisterB->Register_Values.Dval;
                else if (!auTokenPtr->RegisterB->Data_Type_Settings && !auTokenPtr->RegisterA->Data_Type_Settings)
                    auTokenPtr->RegisterA->Data_Type_Settings = 0,
                            auTokenPtr->RegisterA->Register_Values.Lval /= auTokenPtr->RegisterB->Register_Values.Lval;
                else
                    auTokenPtr->RegisterA->Data_Type_Settings = 1,
                            auTokenPtr->RegisterA->Register_Values.Dval /= auTokenPtr->RegisterB->Register_Values.Lval;
                break;
            case INC:
                if (auTokenPtr->RegisterA->Data_Type_Settings)
                    auTokenPtr->RegisterA->Register_Values.Dval += 1;
                else
                    auTokenPtr->RegisterA->Data_Type_Settings = 0,
                            auTokenPtr->RegisterA->Register_Values.Lval += 1;
                break;
            case DEC:
                if (auTokenPtr->RegisterA->Data_Type_Settings)
                    auTokenPtr->RegisterA->Register_Values.Dval -= 1;
                else
                    auTokenPtr->RegisterA->Register_Values.Lval -= 1;
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
                                              line->generalPurposeTokenPtr->Tokens.CMP_Token->RegisterA->Register_Values.Lval);
            double registerBValue = (double) (line->generalPurposeTokenPtr->Tokens.CMP_Token->RegisterB->Data_Type_Settings
                                              ?
                                              line->generalPurposeTokenPtr->Tokens.CMP_Token->RegisterB->Register_Values.Dval
                                              :
                                              line->generalPurposeTokenPtr->Tokens.CMP_Token->RegisterB->Register_Values.Lval);
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
