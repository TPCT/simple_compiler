#include "parser.h"

static String_Constant reservedWords[] = {"MOV", "ADD", "SUB", "DIV", "MUL", "INC", "DEC",
                                          "JMP", "JNE", "JE", "JGE", "JG", "JLE", "JL",
                                          "CMP", "CALL", "MSG", "RET", "END", NULL};
#define GPT line->generalPurposeTokenPtr

void parse(void) {
    Line_Ptr linePtr = Actual_Lines_tree;
    while (linePtr) {
        lineParamChecker(linePtr);
        checkLabelDuplications(linePtr);
        labelNameChecker(linePtr);
        linePtr = linePtr->Next_Line;
    }
}

void checkLabelDuplications(Line_Ptr line) {
    if (!line->Error) {
        if (line->linetype == labelLine) {
            Label_Token_Ptr labelTokenPtr = Labels_Container;
            if (GPT->Tokens.Label_Token->Label_Name) {
                while (labelTokenPtr) {
                    if (!strcmp(labelTokenPtr->Label_Name, GPT->Tokens.Label_Token->Label_Name)
                        &&
                        labelTokenPtr->Label_Address !=
                        GPT->Tokens.Label_Token->Label_Address) {
                        line->Error = makeDuplicateLabelError(line->lineCode);
                        break;
                    }
                    labelTokenPtr = labelTokenPtr->Next_Label;
                }
            }
        }
    }
}

void registerNameChecker(Line_Ptr line, Register_Ptr *registerPtr) {
    if ((line->linetype == auLine || line->linetype == cmpLine || line->linetype == msgLine) &&
        !line->Error) {
        if (*registerPtr) {
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
            if (line->linetype == auLine && (GPT->Tokens.Au_Token->Instruction == INC
                                             || GPT->Tokens.Au_Token->Instruction == DEC));
            else
                line->Error = makeNullRegisterError(line->lineCode);
        }
    }
}

void labelNameChecker(Line_Ptr line) {
    if (!line->Error) {
        if (line->linetype == labelLine) {
            for (int i = 0; reservedWords[i]; i++) {
                if (!strcmp(GPT->Tokens.Label_Token->Label_Name, reservedWords[i])) {
                    line->Error = makeLabelNameError(line->lineCode, GPT->Tokens.Label_Token->Label_Name);
                    return;
                }
            }
            if (paramsLens(GPT->Tokens.Label_Token->Label_Name) > 1) {
                line->Error = makeLabelNameError(line->lineCode, GPT->Tokens.Label_Token->Label_Name);
                return;
            }
        } else if (line->linetype == jmpLine) {
            for (int i = 0; reservedWords[i]; i++) {
                if (!strcmp(GPT->Tokens.Jump_Token->Label_Name, reservedWords[i])) {
                    line->Error = makeLabelNameError(line->lineCode, GPT->Tokens.Jump_Token->Label_Name);
                    return;
                }
            }
            if (paramsLens(GPT->Tokens.Jump_Token->Label_Name) > 1) {
                line->Error = makeLabelNameError(line->lineCode, GPT->Tokens.Jump_Token->Label_Name);
                return;
            }
        } else if (line->linetype == callLine) {
            for (int i = 0; reservedWords[i]; i++) {
                if (!strcmp(GPT->Tokens.Call_Token->labelName, reservedWords[i])) {
                    line->Error = makeLabelNameError(line->lineCode, GPT->Tokens.Call_Token->labelName);
                    return;
                }
            }
            if (paramsLens(GPT->Tokens.Call_Token->labelName) > 1) {
                line->Error = makeLabelNameError(line->lineCode, GPT->Tokens.Call_Token->labelName);
                return;
            }
        }
    }
}

void lineParamChecker(Line_Ptr line) {
    switch (line->linetype) {
        case auLine:
            if (GPT->Tokens.Au_Token->Instruction == INC
                || GPT->Tokens.Au_Token->Instruction == DEC) {
                if (paramsLens(line->lineCode) > 1)
                    line->Error = makeParamError(line->lineCode);
            } else {
                if (paramsLens(line->lineCode) > 2)
                    line->Error = makeParamError(line->lineCode);
            }
            if (GPT->Tokens.Au_Token->RegisterA->registerType == TEMP_REGISTER) {
                line->Error = makeRegisterTypeError("REGISTER A MUST BE A REAL REGISTER NOT A NUMBER",
                                                    GPT->Tokens.Au_Token->RegisterA->registerType);
                break;
            }
            registerNameChecker(line, &GPT->Tokens.Au_Token->RegisterA);
            registerNameChecker(line, &GPT->Tokens.Au_Token->RegisterB);
            break;
        case cmpLine:
            if (paramsLens(line->lineCode) > 2)
                line->Error = makeParamError(line->lineCode);
            registerNameChecker(line, &GPT->Tokens.CMP_Token->RegisterA);
            registerNameChecker(line, &GPT->Tokens.CMP_Token->RegisterB);
            break;
        case jmpLine:
            if (paramsLens(line->lineCode) > 1) {
                line->Error = makeSyntaxError(line->lineCode);
                break;
            }
            if (!GPT->Tokens.Jump_Token->isSet) {
                line->Error = makeUnboundedJumpError(line->lineCode);
                break;
            }
            if (!GPT->Tokens.CMP_Token)
                line->Error = makeUnboundedCmpError(line->lineCode);
            break;
        case callLine:
            if (paramsLens(line->lineCode) > 1) {
                line->Error = makeSyntaxError(line->lineCode);
                break;
            }
            if (!GPT->Tokens.Call_Token->isSet) {
                line->Error = makeUnboundedCallError(line->lineCode);
                break;
            }
            if (!GPT->Tokens.Call_Token->labelLine->generalPurposeTokenPtr->Tokens.Label_Token->hasReturn) {
                line->Error = makeCallWithoutReturn(line->lineCode);
                break;
            }
            break;
        case labelLine:
            if (paramsLens(line->lineCode) > 1) {
                line->Error = makeSyntaxError(line->lineCode);
                break;
            }
            break;
        case msgLine: {
            Msg_String_Ptr msgStringPtr = GPT->Tokens.MSG_Token->String_Tree;
            MSG_Register_Ptr registerPtr = NULL;
            while (msgStringPtr) {
                unsigned length = strlen(msgStringPtr->MSG_TOKEN_STRING);
                if (length >= 2 && *(msgStringPtr->MSG_TOKEN_STRING) == '\'' &&
                    *(msgStringPtr->MSG_TOKEN_STRING + length - 1) == '\'') {
                } else {
                    line->Error = makeSyntaxError(line->lineCode);
                    break;
                }
                msgStringPtr = msgStringPtr->Next_String;
            }
            if (!line->Error) {
                registerPtr = GPT->Tokens.MSG_Token->Registers_Tree;
                while (registerPtr) {
                    registerNameChecker(line, &(registerPtr->REGISTER));
                    registerPtr = registerPtr->NEXT_REGISTER;
                }
            }
        }
            break;
        case returnLine:
            if (paramsLens(line->lineCode) > 1) {
                line->Error = makeSyntaxError(line->lineCode);
                break;
            }
            if (!GPT->Tokens.returnToken) {
                line->Error = makeUnboundedReturnError(line->lineCode);
                break;
            }
            if (!GPT->Tokens.returnToken->Label_Address)
                line->Error = makeInvalidReturnError(line->lineCode);
            break;
        case endLine:
            if (paramsLens(line->lineCode) > 1)
                line->Error = makeSyntaxError(line->lineCode);
            break;
        case undefinedLine:
            line->Error = makeSyntaxError(line->lineCode);
            break;
    }

}

unsigned paramsLens(String code) {
    unsigned paramsLen = 0;
    code = strdup(code);
    String token;
    do {
        token = extractParameter(&code);
        if (token)
            paramsLen++;
        free(token);
    } while (code && token);
    free(code);
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

Error_Ptr makeUnboundedReturnError(String lineCode) {
    Error_Ptr error = (Error_Ptr) malloc(sizeof(Error));
    error->errorType = UNBOUNDED_RETURN_ERROR;
    unsigned length = snprintf(NULL, 0, "UNBOUNDED RETURN ERROR AT LINE %s",
                               lineCode);
    String errorMsg = malloc(sizeof(char) * (length + 1));
    snprintf(errorMsg, length + 1, "UNBOUNDED RETURN ERROR AT LINE %s",
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

Error_Ptr makeCallWithoutReturn(String lineCode) {
    Error_Ptr error = (Error_Ptr) malloc(sizeof(Error));
    error->errorType = CALLING_FUNCTION_WITHOUT_RETURN;
    unsigned length = snprintf(NULL, 0, "CALLING FUNCTION WITHOUT RETURN ERROR AT LINE %s\nUSE JMP INSTRUCTION",
                               lineCode);
    String errorMsg = malloc(sizeof(char) * (length + 1));
    snprintf(errorMsg, length + 1, "CALLING FUNCTION WITHOUT RETURN ERROR AT LINE %s\nUSE JMP INSTRUCTION",
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
