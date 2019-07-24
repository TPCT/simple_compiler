#include "parser.h"

static String_Constant reservedWords[] = {"MOV", "ADD", "SUB", "DIV", "MUL", "INC", "DEC",
                                          "JMP", "JNE", "JE", "JGE", "JG", "JLE", "JL",
                                          "CMP", "CALL", "MSG", "RET", "END", NULL};

void parse(void) {
    Line_Ptr linePtr = Actual_Lines_tree;
    while (linePtr) {
        lineParamChecker(linePtr);
        checkLabelDuplications(linePtr);
        labelNameChecker(linePtr);
        printLine(linePtr);
        linePtr = linePtr->Next_Line;
    }
}

void checkLabelDuplications(Line_Ptr line) {
    if (!line->Error) {
        if (line->generalPurposeTokenPtr->tokenType == LABEL_TOKEN_TYPE) {
            Label_Token_Ptr labelTokenPtr = Labels_Container;
            while (labelTokenPtr) {
                if (!strcmp(labelTokenPtr->Label_Name, line->generalPurposeTokenPtr->Tokens.Label_Token->Label_Name)
                    &&
                    labelTokenPtr->Label_Address != line->generalPurposeTokenPtr->Tokens.Label_Token->Label_Address) {
                    line->Error = makeDuplicateLabelError(line->lineCode);
                    break;
                }
                labelTokenPtr = labelTokenPtr->Next_Label;
            }
        }
    }
}

void registerNameChecker(Line_Ptr line, Register_Ptr *registerPtr) {
#define GPT line->generalPurposeTokenPtr
    if ((GPT->tokenType == AU_TOKEN_TYPE || GPT->tokenType == CMP_TOKEN_TYPE || GPT->tokenType == MSG_TOKEN_TYPE) &&
        !line->Error) {
        if (registerPtr) {
            for (int i = 0; reservedWords[i]; i++) {
                if (!strcmp((*registerPtr)->Register_Name, reservedWords[i])) {
                    line->Error = makeRegisterNamingError(line->lineCode,
                                                          (*registerPtr)->Register_Name);
                    return;
                }
            }
            for (unsigned i = 0, c; (c = *((*registerPtr)->Register_Name + i)); i++) {
                if (((!i && isdigit(c))) || c == ' ') {
                    line->Error = makeRegisterNamingError(line->lineCode,
                                                          (*registerPtr)->Register_Name);
                    return;
                }
            }
            if ((*registerPtr)->registerType == UNDEFINED_REGISTER) {
                Register_Ptr tempSavingRegister = *registerPtr;
                Register_Ptr tempRegister = searchRegister(&tempSavingRegister);
                *registerPtr = tempRegister;
                if ((*registerPtr)->registerType == UNDEFINED_REGISTER) {
                    line->Error
                            = makeUndefinedRegisterError(line->lineCode, (*registerPtr)->Register_Name);
                } else {
                    if (line->Error)
                        Error_Counter--,
                                free(line->Error),
                                line->Error = NULL;
                }
            }
        } else {
            if (GPT->tokenType == AU_TOKEN_TYPE && (GPT->Tokens.Au_Token->Instruction != INC
                                                    && GPT->Tokens.Au_Token->Instruction != DEC))
                line->Error = makeNullRegisterError(line->lineCode);
        }
    }
}

void labelNameChecker(Line_Ptr line) {
    if (!line->Error) {
        if (GPT->tokenType == LABEL_TOKEN_TYPE) {
            for (int i = 0; reservedWords[i]; i++) {
                if (!strcmp(GPT->Tokens.Label_Token->Label_Name, reservedWords[i])) {
                    line->Error = makeLabelNameError(line->lineCode, GPT->Tokens.Label_Token->Label_Name);
                    return;
                }
            }
            if (paramsLens(strdup(GPT->Tokens.Label_Token->Label_Name)) > 1) {
                line->Error = makeLabelNameError(line->lineCode, GPT->Tokens.Label_Token->Label_Name);
                return;
            }
        } else if (GPT->tokenType == JUMP_TOKEN_TYPE) {
            for (int i = 0; reservedWords[i]; i++) {
                if (!strcmp(GPT->Tokens.Jump_Token->Label_Name, reservedWords[i])) {
                    line->Error = makeLabelNameError(line->lineCode, GPT->Tokens.Jump_Token->Label_Name);
                    return;
                }
            }
            if (paramsLens(strdup(GPT->Tokens.Jump_Token->Label_Name)) > 1) {
                line->Error = makeLabelNameError(line->lineCode, GPT->Tokens.Jump_Token->Label_Name);
                return;
            }
        } else if (GPT->tokenType == CALL_TOKEN_TYPE) {
            for (int i = 0; reservedWords[i]; i++) {
                if (!strcmp(GPT->Tokens.Call_Token->Label_Name, reservedWords[i])) {
                    line->Error = makeLabelNameError(line->lineCode, GPT->Tokens.Call_Token->Label_Name);
                    return;
                }
            }
            if (paramsLens(strdup(GPT->Tokens.Call_Token->Label_Name)) > 1) {
                line->Error = makeLabelNameError(line->lineCode, GPT->Tokens.Call_Token->Label_Name);
                return;
            }
        }
    }
}

void lineParamChecker(Line_Ptr line) {
    switch (line->generalPurposeTokenPtr->tokenType) {
        case AU_TOKEN_TYPE:
            if (line->generalPurposeTokenPtr->Tokens.Au_Token->Instruction == INC
                || line->generalPurposeTokenPtr->Tokens.Au_Token->Instruction == DEC) {
                if (paramsLens(strdup(line->lineCode)) > 1)
                    line->Error = makeParamError(line->lineCode);
            } else {
                if (paramsLens(strdup(line->lineCode)) > 2)
                    line->Error = makeParamError(line->lineCode);
            }
            printf("%s %s\n", line->generalPurposeTokenPtr->Tokens.Au_Token->RegisterA->Register_Name,
                   Register_TYPES[line->generalPurposeTokenPtr->Tokens.Au_Token->RegisterA->registerType]);
            if (line->generalPurposeTokenPtr->Tokens.Au_Token->RegisterA->registerType == TEMP_REGISTER) {
                line->Error = makeRegisterTypeError("REGISTER A MUST BE A REAL REGISTER NOT A NUMBER",
                                                    line->generalPurposeTokenPtr->Tokens.Au_Token->RegisterA->registerType);
                break;
            }
            registerNameChecker(line, &line->generalPurposeTokenPtr->Tokens.Au_Token->RegisterA);
            registerNameChecker(line, &line->generalPurposeTokenPtr->Tokens.Au_Token->RegisterB);
            break;
        case CMP_TOKEN_TYPE:
            if (paramsLens(strdup(line->lineCode)) > 2)
                line->Error = makeParamError(line->lineCode);
            registerNameChecker(line, &line->generalPurposeTokenPtr->Tokens.CMP_Token->RegisterA);
            registerNameChecker(line, &line->generalPurposeTokenPtr->Tokens.CMP_Token->RegisterB);
            break;
        case JUMP_TOKEN_TYPE:
            if (paramsLens(strdup(line->lineCode)) > 1) {
                line->Error = makeSyntaxError(line->lineCode);
                break;
            }
            if (!line->generalPurposeTokenPtr->Tokens.Jump_Token->Line_Address) {
                line->Error = makeUnboundedJumpError(line->lineCode);
                break;
            }
            if (!line->generalPurposeTokenPtr->Tokens.CMP_Token)
                line->Error = makeUnboundedCmpError(line->lineCode);
            break;
        case END_TOKEN_TYPE:
            if (paramsLens(strdup(line->lineCode)) > 1)
                line->Error = makeSyntaxError(line->lineCode);
            break;
        case RETURN_TOKEN_TYPE:
            if (paramsLens(strdup(line->lineCode)) > 1) {
                line->Error = makeSyntaxError(line->lineCode);
                break;
            }
            if (!line->generalPurposeTokenPtr->Tokens.Return_Token->callToken)
                line->Error = makeInvalidReturnError(line->lineCode);
            break;
        case CALL_TOKEN_TYPE:
            line->generalPurposeTokenPtr->Tokens.Call_Token->isCalling = 0;
            if (paramsLens(strdup(line->lineCode)) > 1) {
                line->Error = makeSyntaxError(line->lineCode);
                break;
            }
            if (!line->generalPurposeTokenPtr->Tokens.Call_Token->RET_TOKEN) {
                line->Error = makeReturnTokenError(line->lineCode);
                break;
            }
            if (!line->generalPurposeTokenPtr->Tokens.Call_Token->Line_Address)
                line->Error = makeUnboundedCallError(line->lineCode);
            break;
        case LABEL_TOKEN_TYPE:
            if (paramsLens(strdup(line->lineCode)) > 1) {
                line->Error = makeSyntaxError(line->lineCode);
                break;
            }
            break;
        case MSG_TOKEN_TYPE: {
            Msg_String_Ptr msgStringPtr = line->generalPurposeTokenPtr->Tokens.MSG_Token->String_Tree;
            while (msgStringPtr) {
                unsigned length = stringTrim(&msgStringPtr->MSG_TOKEN_STRING);
                if (*(msgStringPtr->MSG_TOKEN_STRING) == '\'' &&
                    *(msgStringPtr->MSG_TOKEN_STRING + length - 1) == '\'') {
                } else {
                    line->Error = makeSyntaxError(line->lineCode);
                    break;
                }
                msgStringPtr = msgStringPtr->Next_String;
            }
            if (!line->Error) {
                MSG_Register_Ptr registerPtr = line->generalPurposeTokenPtr->Tokens.MSG_Token->Registers_Tree;
                while (registerPtr) {
                    registerNameChecker(line, &registerPtr->REGISTER);
                    registerPtr = registerPtr->NEXT_REGISTER;
                }
            }
        }
            break;
        case UNIDENTIFIED_TOKEN_TYPE:
            line->Error = makeSyntaxError(line->lineCode);
            break;
    }

}

unsigned paramsLens(String code) {
    unsigned paramsLen = 0;
    while (code) {
        strsep(&code, ",");
        paramsLen++;
    }
    return paramsLen;
}

Error_Ptr makeRegisterNamingError(String lineCode, String name) {
    Error_Ptr error = (Error_Ptr) malloc(sizeof(Error));
    error->errorType = UNDEFINED_REGISTER_NAME;
    unsigned length = snprintf(NULL, 0, "NAMING ERROR AT LINE %s REGISTER NAME: %s",
                               lineCode, name);
    String errorMsg = malloc(sizeof(char) * (length + 1));
    snprintf(errorMsg, length + 1, "NAMING ERROR AT LINE %s REGISTER NAME: %s",
             lineCode, name);
    error->Error_MSG = errorMsg;
    Error_Counter++;
    return error;
}

Error_Ptr makeUndefinedRegisterError(String lineCode, String name) {
    Error_Ptr error = (Error_Ptr) malloc(sizeof(Error));
    error->errorType = NAMING_ERROR;
    unsigned length = snprintf(NULL, 0, "UNDEFINED REGISTER ERROR AT LINE %s REGISTER NAME: %s",
                               lineCode, name);
    String errorMsg = malloc(sizeof(char) * (length + 1));
    snprintf(errorMsg, length + 1, "UNDEFINED REGISTER ERROR AT LINE %s REGISTER NAME: %s",
             lineCode, name);
    error->Error_MSG = errorMsg;
    Error_Counter++;
    return error;
}

Error_Ptr makeLabelNameError(String lineCode, String name) {
    Error_Ptr error = (Error_Ptr) malloc(sizeof(Error));
    error->errorType = NAMING_ERROR;
    unsigned length = snprintf(NULL, 0, "NAMING ERROR AT LINE %s LABEL NAME: %s",
                               lineCode, name);
    String errorMsg = malloc(sizeof(char) * (length + 1));
    snprintf(errorMsg, length + 1, "NAMING ERROR AT LINE %s LABEL NAME: %s",
             lineCode, name);
    error->Error_MSG = errorMsg;
    Error_Counter++;
    return error;
}

Error_Ptr makeNullRegisterError(String lineCode) {
    Error_Ptr error = (Error_Ptr) malloc(sizeof(Error));
    error->errorType = NULL_REGISTER_ERROR;
    unsigned length = snprintf(NULL, 0, "NULL REGISTER AT LINE %s REGISTER NAME: (nil)",
                               lineCode);
    String errorMsg = malloc(sizeof(char) * (length + 1));
    snprintf(errorMsg, length + 1, "NULL REGISTER AT LINE %s REGISTER NAME: (nil)",
             lineCode);
    error->Error_MSG = errorMsg;
    Error_Counter++;
    return error;
}

Error_Ptr makeParamError(String lineCode) {
    Error_Ptr error = (Error_Ptr) malloc(sizeof(Error));
    error->errorType = PARAMETRIZATION_ERROR;
    unsigned length = snprintf(NULL, 0, "PARAMETRIZATION ERROR AT LINE %s",
                               lineCode);
    String errorMsg = malloc(sizeof(char) * (length + 1));
    snprintf(errorMsg, length + 1, "PARAMETRIZATION ERROR AT LINE %s",
             lineCode);
    error->Error_MSG = errorMsg;
    Error_Counter++;
    return error;
}

Error_Ptr makeSyntaxError(String lineCode) {
    Error_Ptr error = (Error_Ptr) malloc(sizeof(Error));
    error->errorType = SYNTAX_ERROR;
    unsigned length = snprintf(NULL, 0, "SYNTAX ERROR AT LINE %s",
                               lineCode);
    String errorMsg = malloc(sizeof(char) * (length + 1));
    snprintf(errorMsg, length + 1, "SYNTAX ERROR AT LINE %s",
             lineCode);
    error->Error_MSG = errorMsg;
    Error_Counter++;
    return error;
}

Error_Ptr makeDuplicateLabelError(String lineCode) {
    Error_Ptr error = (Error_Ptr) malloc(sizeof(Error));
    error->errorType = DUPLICATED_LABEL_ERROR;
    unsigned length = snprintf(NULL, 0, "DUPLICATED LABEL NAME ERROR AT LINE %s",
                               lineCode);
    String errorMsg = malloc(sizeof(char) * (length + 1));
    snprintf(errorMsg, length + 1, "DUPLICATED LABEL NAME ERROR AT LINE %s",
             lineCode);
    error->Error_MSG = errorMsg;
    Error_Counter++;
    return error;
}

Error_Ptr makeUnboundedCallError(String lineCode) {
    Error_Ptr error = (Error_Ptr) malloc(sizeof(Error));
    error->errorType = UNBOUNDED_CALL_CONDITION;
    unsigned length = snprintf(NULL, 0, "UNBOUNDED CALL ERROR AT LINE %s",
                               lineCode);
    String errorMsg = malloc(sizeof(char) * (length + 1));
    snprintf(errorMsg, length + 1, "UNBOUNDED CALL ERROR AT LINE %s",
             lineCode);
    error->Error_MSG = errorMsg;
    Error_Counter++;
    return error;
}

Error_Ptr makeUnboundedJumpError(String lineCode) {
    Error_Ptr error = (Error_Ptr) malloc(sizeof(Error));
    error->errorType = UNBOUNDED_JUMP_CONDITION;
    unsigned length = snprintf(NULL, 0, "UNBOUNDED JUMP ERROR AT LINE %s",
                               lineCode);
    String errorMsg = malloc(sizeof(char) * (length + 1));
    snprintf(errorMsg, length + 1, "UNBOUNDED JUMP ERROR AT LINE %s",
             lineCode);
    error->Error_MSG = errorMsg;
    Error_Counter++;
    return error;
}

Error_Ptr makeUnboundedCmpError(String lineCode) {
    Error_Ptr error = (Error_Ptr) malloc(sizeof(Error));
    error->errorType = UNBOUNDED_CMP_ERROR;
    unsigned length = snprintf(NULL, 0, "UNBOUNDED CMP ERROR AT LINE %s",
                               lineCode);
    String errorMsg = malloc(sizeof(char) * (length + 1));
    snprintf(errorMsg, length + 1, "UNBOUNDED CMP ERROR AT LINE %s",
             lineCode);
    error->Error_MSG = errorMsg;
    Error_Counter++;
    return error;
}

Error_Ptr makeRegisterTypeError(String lineCode, Register_Type type) {
    Error_Ptr error = (Error_Ptr) malloc(sizeof(Error));
    error->errorType = REGISTER_TYPE_ERROR;
    unsigned length = snprintf(NULL, 0, "REGISTER TYPE ERROR AT LINE %s\n    REGISTER TYPE: %s",
                               lineCode, Register_TYPES[type]);
    String errorMsg = malloc(sizeof(char) * (length + 1));
    snprintf(errorMsg, length + 1, "REGISTER TYPE ERROR AT LINE %s\n    REGISTER TYPE: %s",
             lineCode, Register_TYPES[type]);
    error->Error_MSG = errorMsg;
    Error_Counter++;
    return error;
}

Error_Ptr makeInvalidReturnError(String lineCode) {
    Error_Ptr error = (Error_Ptr) malloc(sizeof(Error));
    error->errorType = INVALID_RETURN_TOKEN_ERROR;
    unsigned length = snprintf(NULL, 0, "INVALID RETURN ERROR AT LINE %s",
                               lineCode);
    String errorMsg = malloc(sizeof(char) * (length + 1));
    snprintf(errorMsg, length + 1, "INVALID RETURN ERROR AT LINE %s",
             lineCode);
    error->Error_MSG = errorMsg;
    Error_Counter++;
    return error;
}

Error_Ptr makeReturnTokenError(String lineCode) {
    Error_Ptr error = (Error_Ptr) malloc(sizeof(Error));
    error->errorType = UNBOUNDED_RETURN_CALL_ERROR;
    unsigned length = snprintf(NULL, 0, "UNBOUNDED RETURN CALL ERROR AT LINE %s",
                               lineCode);
    String errorMsg = malloc(sizeof(char) * (length + 1));
    snprintf(errorMsg, length + 1, "UNBOUNDED RETURN CALL ERROR AT LINE %s",
             lineCode);
    error->Error_MSG = errorMsg;
    Error_Counter++;
    return error;
}



