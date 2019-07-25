#include "lexer.h"
#include "tokens.h"

static String AU_KEYWORDS[] = {"MOV", "ADD", "SUB", "DIV", "MUL", "INC", "DEC", NULL};
static String JUMP_KEYWORDS[] = {"JMP", "JNE", "JE", "JGE", "JG", "JLE", "JL", NULL};
static String_Constant CMP = "CMP";
static String_Constant CALL = "CALL"; /* For Calling Subroutine */
static String_Constant MSG = "MSG";   /* For Printing Output */
static String_Constant RET = "RET";   /* For Returning To The Last Point and Exit From SubRoutine */
static String_Constant END = "END";   /* For end of program detection */

void codeReader(String code) {
    do {
        String token = extractLine(&code);
        beautify(&token);
        if (token)
            lineMaker(token);
    } while (code);
    assignLabelsToJumps();
    assignLabelsToCalls();
}

void assignLabelsToJumps(void) {
    Label_Token_Ptr labelTokenPtr = Labels_Container;
    Jump_Token_Ptr *tempPtr = &TEMP_JUMPS_CONTAINER;
    if (!tempPtr)
        return;
    while (labelTokenPtr) {
        if (!*tempPtr)
            return;
        while (*tempPtr) {
            if (!strcmp(labelTokenPtr->Label_Name, (*tempPtr)->Label_Name)) {
                (*tempPtr)->isSet = True;
                (*tempPtr)->Line_Address = labelTokenPtr->Label_Address;
                (*tempPtr) = (*tempPtr)->Next_Jump;
                continue;
            }
            tempPtr = &(*tempPtr)->Next_Jump;
        }
        tempPtr = &TEMP_JUMPS_CONTAINER;
        labelTokenPtr = labelTokenPtr->Next_Label;
    }
}

void assignLabelsToCalls(void) {
    Label_Token_Ptr labelTokenPtr = Labels_Container;
    Calls_Tree_Ptr *tempPtr = &TEMP_CALLS_CONTAINER;
    Calls_Tree_Ptr lastCall = NULL;
    while (labelTokenPtr && *tempPtr) {
        while (*tempPtr) {
            if (!strcmp(labelTokenPtr->Label_Name, (*tempPtr)->call->labelName)) {
                (*tempPtr)->call->labelLine = labelTokenPtr->Label_Address;
                (*tempPtr)->call->isSet = True;
                (*tempPtr)->call->callLine = &labelTokenPtr->last_caller;
                (*tempPtr)->call->callLabel = &labelTokenPtr->called;
                *LAST_BOUNDED_CALL_CONTAINER = (*tempPtr)->call;
                if (lastCall && lastCall->Next_Node) {
                    lastCall->Next_Node = (*tempPtr)->Next_Node;
                    free(*tempPtr);
                    lastCall = lastCall->Next_Node;
                    tempPtr = &lastCall;
                    continue;
                }
                tempPtr = &(*tempPtr)->Next_Node;
                continue;
            }
            lastCall = *tempPtr;
            tempPtr = &(*tempPtr)->Next_Node;
        }
        tempPtr = &TEMP_CALLS_CONTAINER;
        labelTokenPtr = labelTokenPtr->Next_Label;
    }
}

void addJump(Jump_Token_Ptr *savingPtr, Jump_Token_Ptr *jumpPtr) {
    Jump_Token_Ptr *installingContainer = &TEMP_JUMPS_CONTAINER;
    while (*installingContainer) {
        if (((*installingContainer)->Instruction == (*jumpPtr)->Instruction) &&
            !strcmp((*installingContainer)->Label_Name, (*jumpPtr)->Label_Name)) {
            *savingPtr = *installingContainer,
                    free(*jumpPtr);
            return;
        }
        installingContainer = &(*installingContainer)->Next_Jump;
    }
    *installingContainer = *jumpPtr;
    (*installingContainer)->Next_Jump = NULL;
    *savingPtr = *installingContainer;
}

void addCall(Call_Token_Ptr *savingPtr, Call_Token_Ptr *callPtr) {
    Calls_Tree_Ptr *installingContainer = &TEMP_CALLS_CONTAINER;
    while (*installingContainer) {
        if (!strcmp((*installingContainer)->call->labelName, (*callPtr)->labelName)) {
            *savingPtr = (*installingContainer)->call,
                    free(*callPtr);
            break;
        }
        installingContainer = &(*installingContainer)->Next_Node;
    }
    if (!*installingContainer)
        *installingContainer = (Calls_Tree_Ptr) malloc(sizeof(Calls_Tree));
    (*installingContainer)->call = *callPtr;
    (*installingContainer)->Next_Node = NULL;
    *savingPtr = (*installingContainer)->call;
}

void addLabel(Label_Token_Ptr *label) {
    *LAST_FREE_Labels_Container = *label;
    LAST_FREE_Labels_Container = &(*LAST_FREE_Labels_Container)->Next_Label;
}

void lineMaker(String token) {
    Line_Ptr line;
    line = (Line_Ptr) calloc(1, sizeof(Line));
    line->lineCode = token;
    tokenize(token, line);
    if (line) {
        Actual_Lines_tree = addLine(&Actual_Lines_tree, &line);
    }
}

void tokenize(String lineString, Line_Ptr line) {
    if (lineString) {
        unsigned char isLbl = 0;
        String token = NULL;
        line->lineCode = strdup(lineString);
        token = extractCmd(&lineString, &isLbl);
        if (token) {
            line->Instruction_String = strdup(token);
            line->generalPurposeTokenPtr = (GENERAL_PURPOSE_TOKEN_PTR) calloc(1, sizeof(GENERAL_PURPOSE_TOKEN));
            line->Error = NULL;
            if (!isLbl) {
                for (char i = 0; AU_KEYWORDS[i] || JUMP_KEYWORDS[i < 7 ? i : 7]; i++) {
                    if (!strcmp(AU_KEYWORDS[i], line->Instruction_String)) {
                        makeAuToken(line, lineString, i);
                        free(token);
                        free(lineString);
                        return;
                    } else if ((i < 7) && !strcmp(JUMP_KEYWORDS[i], line->Instruction_String)) {
                        makeJumpToken(line, lineString, i);
                        free(token);
                        free(lineString);
                        return;
                    }
                }
            }
            if (!strcmp(token, CALL))
                makeCallToken(line, lineString);
            else if (!strcmp(token, CMP))
                makeCmpToken(line, lineString);
            else if (!strcmp(token, END))
                line->linetype = endLine;
            else if (!strcmp(token, RET))
                makeReturnToken(line);
            else if (!strcmp(token, MSG)) {
                makeMsgToken(line, lineString);
            } else if (isLbl) {
                makeLabelToken(line);
            } else
                makeUndefinedToken(line);
            line->Next_Line = NULL;
            free(token);
        }
        free(lineString);
    }
}

void tempRegisterPtrAdd(Register_Ptr *savingRegister, Register_Ptr *registerPtr) {
    Register_Ptr tempRegister = searchRegister(registerPtr);
    if (*registerPtr)
        *LAST_FREE_REGISTER_IN_TEMP_REGISTER_TREE = *registerPtr,
        LAST_FREE_REGISTER_IN_TEMP_REGISTER_TREE = &(*LAST_FREE_REGISTER_IN_TEMP_REGISTER_TREE)->Next_Register,
        *savingRegister = *registerPtr;
    else
        *savingRegister = tempRegister;
}

void actualRegisterPtrAdd(Register_Ptr *savingRegister, Register_Ptr *registerPtr) {
    Register_Ptr tempRegister = searchRegister(registerPtr);
    if (*registerPtr)
        *LAST_FREE_REGISTER_IN_ACTUAL_REGISTER_TREE = *registerPtr,
        LAST_FREE_REGISTER_IN_ACTUAL_REGISTER_TREE = &(*LAST_FREE_REGISTER_IN_ACTUAL_REGISTER_TREE)->Next_Register,
        *savingRegister = *registerPtr;
    else
        *savingRegister = tempRegister;
}

void makeCallToken(Line_Ptr line, String_Constant lineStringPtr) {
    Call_Token_Ptr callTokenPtr = (Call_Token_Ptr) calloc(1, sizeof(Call_Token));
    String tempData = strdup(lineStringPtr);
    callTokenPtr->isSet = False;
    callTokenPtr->labelName = extractCmd(&tempData, NULL);
    callTokenPtr->callLine = NULL;
    callTokenPtr->labelLine = NULL;
    callTokenPtr->callLabel = NULL;
    line->linetype = callLine;
    addCall(&line->generalPurposeTokenPtr->Tokens.Call_Token, &callTokenPtr);
    free(tempData);
}

void makeCmpToken(Line_Ptr line, String_Constant lineStringPtr) {
    String tempData = strdup(lineStringPtr);
    LAST_CMP = (CMP_Token_Ptr) calloc(1, sizeof(CMP_Token));
    LAST_CMP->CMP_Val = EQUAL;
    Register_Ptr *registerPtr = (Register_Ptr *) malloc(sizeof(Register_Ptr));
    *registerPtr = extractRegister(&tempData);
    Register_Ptr tempRegister = searchRegister(registerPtr);
    LAST_CMP->RegisterA = tempRegister;
    if (!*registerPtr || (*registerPtr && (*registerPtr)->registerType == TEMP_REGISTER));
    else
        LAST_CMP->RegisterA->registerType = UNDEFINED_REGISTER;
    *registerPtr = extractRegister(&tempData);
    tempRegister = searchRegister(registerPtr);
    LAST_CMP->RegisterB = tempRegister;
    if (!*registerPtr || (*registerPtr && (*registerPtr)->registerType == TEMP_REGISTER));
    else
        LAST_CMP->RegisterB->registerType = UNDEFINED_REGISTER;
    free(registerPtr);
    line->linetype = cmpLine;
    line->generalPurposeTokenPtr->Tokens.CMP_Token = LAST_CMP;
    free(tempData);
}

void makeMsgToken(Line_Ptr line, String_Constant lineStringPtr) {
    String tempData = strdup(lineStringPtr);
    Msg_Token_Ptr msgTokenPtr = (Msg_Token_Ptr) calloc(1, sizeof(Msg_Token));
    msgTokenPtr->String_Tree = NULL;
    msgTokenPtr->Registers_Tree = NULL;
    Register_Ptr tempRegister = NULL;
    Msg_String_Ptr *tempMsgStringPtr = &msgTokenPtr->String_Tree;
    MSG_Register_Ptr *msgRegisterPtr = &msgTokenPtr->Registers_Tree;
    msgTokenPtr->MSG_BODY = strdup(lineStringPtr);
    String token = NULL;
    String tempToken = NULL;
    unsigned int length = 0;
    do {
        token = extractParameter(&tempData);
        if (!token)
            break;
        length = strlen(token);
        if (*token == '\'' || *(token + length - 1) == '\'') {
            *tempMsgStringPtr = (Msg_String_Ptr) calloc(1, sizeof(MSG_STRING));
            (*tempMsgStringPtr)->MSG_TOKEN_STRING = strdup(token);
            tempToken = strReplace(token, "'", "");
            (*tempMsgStringPtr)->message_string = strdup(tempToken);
            free(tempToken);
            (*tempMsgStringPtr)->Next_String = NULL;
            tempMsgStringPtr = &(*tempMsgStringPtr)->Next_String;
        } else {
            (*msgRegisterPtr) = (MSG_Register_Ptr) calloc(1, sizeof(MSG_REGISTER));
            (*msgRegisterPtr)->REGISTER = extractRegister(&token);
            tempRegister = searchRegister(&(*msgRegisterPtr)->REGISTER);
            if (!(*msgRegisterPtr)->REGISTER)
                (*msgRegisterPtr)->REGISTER = tempRegister;
            else
                (*msgRegisterPtr)->REGISTER->registerType = UNDEFINED_REGISTER;
            msgRegisterPtr = &(*msgRegisterPtr)->NEXT_REGISTER;
        }
        free(token);
    } while (tempData);
    line->linetype = msgLine;
    line->generalPurposeTokenPtr->Tokens.MSG_Token = msgTokenPtr;
    free(tempData);
}

void makeAuToken(Line_Ptr line, String_Constant lineStringPtr, Au_Instructions i) {
    Au_Token_Ptr auTokenPtr = (Au_Token_Ptr) calloc(1, sizeof(Au_Token));
    String tempData = strdup(lineStringPtr);
    line->linetype = auLine;
    line->generalPurposeTokenPtr->Tokens.Au_Token = auTokenPtr;
    line->generalPurposeTokenPtr->Tokens.Au_Token->Instruction = i;
    if (line->generalPurposeTokenPtr->Tokens.Au_Token->Instruction == MOV) {
        makeRegister(&auTokenPtr->RegisterA, &tempData);
        makeRegister(&auTokenPtr->RegisterB, &tempData);
        if (line->generalPurposeTokenPtr->Tokens.Au_Token->RegisterA->registerType != TEMP_REGISTER)
            line->generalPurposeTokenPtr->Tokens.Au_Token->RegisterA->registerType = SAVED_REGISTER;
    } else if (line->generalPurposeTokenPtr->Tokens.Au_Token->Instruction == INC
               || line->generalPurposeTokenPtr->Tokens.Au_Token->Instruction == DEC) {
        specialMakeRegister(&auTokenPtr->RegisterA, &tempData);
        auTokenPtr->RegisterB = NULL;
    } else {
        specialMakeRegister(&auTokenPtr->RegisterA, &tempData);
        specialMakeRegister(&auTokenPtr->RegisterB, &tempData);
    }
    free(tempData);
}

void makeJumpToken(Line_Ptr line, String lineStringPtr, Jump_Instruction i) {
    //assignLabelsToJumps();
    lineStringPtr = strReplace(lineStringPtr, ",", "");
    Jump_Token_Ptr jumpTokenPtr = (Jump_Token_Ptr) calloc(1, sizeof(Jump_Token));
    jumpTokenPtr->Label_Name = extractCmd(&lineStringPtr, NULL);
    jumpTokenPtr->Instruction = i;
    jumpTokenPtr->CMP_Token = LAST_CMP;
    line->linetype = jmpLine;
    addJump(&line->generalPurposeTokenPtr->Tokens.Jump_Token, &jumpTokenPtr);
    free(lineStringPtr);
}

void specialMakeRegister(Register_Ptr *regPtr, String *code) {
    Register_Ptr *registerPtr = (Register_Ptr *) malloc(sizeof(Register_Ptr));
    *registerPtr = extractRegister(code);
    Register_Ptr tempRegister = searchRegister(registerPtr);
    (*regPtr) = tempRegister;
    if (!*registerPtr || (*registerPtr && (*registerPtr)->registerType == TEMP_REGISTER));
    else {
        (*registerPtr)->registerType = UNDEFINED_REGISTER;
    }
    free(registerPtr);
}

void makeRegister(Register_Ptr *regPtr, String *code) {
    Register_Ptr *tempRegister = (Register_Ptr *) malloc(sizeof(Register_Ptr));
    *tempRegister = extractRegister(code);
    if (!strcmp((*tempRegister)->Register_Name, "TEMP"))
        (*tempRegister)->registerType = TEMP_REGISTER,
                tempRegisterPtrAdd(regPtr, tempRegister);
    else
        actualRegisterPtrAdd(regPtr, tempRegister);
    free(tempRegister);
}

void makeUndefinedToken(Line_Ptr line) {
    line->linetype = undefinedLine;
}

void makeLabelToken(Line_Ptr line) {
    Label_Token_Ptr labelTokenPtr = (Label_Token_Ptr) calloc(1, sizeof(Label_Token));
    line->linetype = labelLine;
    line->generalPurposeTokenPtr->Tokens.Label_Token = labelTokenPtr;
    line->generalPurposeTokenPtr->Tokens.Label_Token->Label_Address = line;
    line->generalPurposeTokenPtr->Tokens.Label_Token->Next_Label = NULL;
    line->generalPurposeTokenPtr->Tokens.Label_Token->Label_Name = strReplace(line->Instruction_String, ":", "");
    line->generalPurposeTokenPtr->Tokens.Label_Token->last_caller = NULL;
    line->generalPurposeTokenPtr->Tokens.Label_Token->called = 0;
    Last_LABEL = labelTokenPtr;
    addLabel(&labelTokenPtr);
}

void makeReturnToken(Line_Ptr line) {
    line->linetype = returnLine;
    line->generalPurposeTokenPtr->Tokens.returnToken = Last_LABEL;
    Last_LABEL = NULL;
}

Register_Ptr extractRegister(String *code) {
    char is_dbl = 0;
    double dnum = 0, numberDivisor = 10;
    long long llnum = 0;
    Register_Ptr regPtr = NULL;
    String token = NULL;
    if (*code) {
        token = extractParameter(code);
        regPtr = (Register_Ptr) malloc(sizeof(Register));
        regPtr->Next_Register = NULL;
        if (token && *token) {
            if (isdigit(*token))
                for (int i = 0; *(token + i); i++) {
                    if (isdigit(*(token + i))) {
                        if (!is_dbl) {
                            dnum *= 10;
                            dnum += *(token + i) - '0';
                            llnum = (long long) dnum;
                        } else {
                            dnum += (*(token + i) - '0') / numberDivisor;
                            numberDivisor *= 10;
                        }
                    } else if (*(token + i) == '.') {
                        is_dbl = 1;
                    } else {
                        goto jumping_label;
                    }
                }
            else {
                jumping_label:
                regPtr->Register_Name = strdup(token);
                regPtr->registerType = SAVED_REGISTER;
                regPtr->Data_Type_Settings = 0;
                regPtr->Register_Values.Lval = 0;
                free(token);
                return regPtr;
            }
            regPtr->Register_Name = "TEMP";
            regPtr->registerType = TEMP_REGISTER;
            if (is_dbl)
                regPtr->Data_Type_Settings = 1,
                        regPtr->Register_Values.Dval = dnum;
            else
                regPtr->Data_Type_Settings = 0,
                        regPtr->Register_Values.Lval = llnum;
        }
    }
    free(token);
    return regPtr;
}

Line_Ptr addLine(Line_Ptr *installLine, Line_Ptr *line) {
    if (!*installLine) {
        *installLine = *line;
        (*installLine)->Next_Line = NULL;
        Lines_Counter += 1;
    } else {
        (*installLine)->Next_Line = addLine(&(*installLine)->Next_Line, line);
    }
    return *installLine;
}

/* TODO:
 * LAST STAGE MACHINE CODE GENERATOR
*/
