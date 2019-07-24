#include "Requirements.h"

String_Constant CMP_RETURNS[] = {"EQUAL", "GREATER THAN", "LESS THAN"};
String_Constant Register_TYPES[] = {"SAVED REGISTER", "UNDEFINED REGISTER", "TEMP REGISTER"};
String_Constant ERRORS_TYPES[] = {"SYNTAX ERROR",
                                  "NAMING ERROR",
                                  "NULL REGISTER ERROR",
                                  "PARAMETRIZATION ERROR",
                                  "UNBOUNDED JUMP CONDITION ERROR",
                                  "UNBOUNDED CALL CONDITION ERROR",
                                  "INVALID RETURN TOKEN ERROR",
                                  "DUPLICATED LABEL NAME ERROR",
                                  "REGISTER TYPE ERROR",
                                  "UNBOUNDED RETURN CALL ERROR",
                                  "UNIDENTIFIED REGISTER NAME ERROR",
                                  "UNBOUNDED CMP TOKEN ERROR"};

String strReplace(String string, String substr, String replacement) {
    char *tok = NULL;
    char *newstr = NULL;
    char *oldstr = NULL;
    int oldstr_len = 0;
    int substr_len = 0;
    int replacement_len = 0;
    newstr = strdup(string);
    substr_len = strlen(substr);
    replacement_len = strlen(replacement);
    if (substr == NULL || replacement == NULL) {
        return newstr;
    }
    while ((tok = strstr(newstr, substr))) {
        oldstr = newstr;
        oldstr_len = strlen(oldstr);
        newstr = (char *) malloc(sizeof(char) * (oldstr_len - substr_len + replacement_len + 1));

        if (newstr == NULL) {
            free(oldstr);
            return NULL;
        }
        memcpy(newstr, oldstr, tok - oldstr);
        memcpy(newstr + (tok - oldstr), replacement, replacement_len);
        memcpy(newstr + (tok - oldstr) + replacement_len, tok + substr_len, oldstr_len - substr_len - (tok - oldstr));
        memset(newstr + oldstr_len - substr_len + replacement_len, 0, 1);
        free(oldstr);
    }
    return newstr;
}

String extractCmd(String *line) {
    if (!line || !*line || !**line) {
        if (line && *line && !**line)
            *line = NULL;
        return NULL;
    }
    unsigned int max_size = 10;
    String cmd = (String) calloc(max_size, sizeof(char));
    for (int i = 0; *(*line); i++) {
        if (*(*line) == ' ') {
            (*line)++;
            *(cmd + i) = '\0';
            return cmd;
        } else {
            if (i == max_size - 2) {
                max_size += 10;
                cmd = (String) realloc(cmd, sizeof(char) * (max_size));
            }
            *(cmd + i) = *((*line)++);
        }
    }
    return cmd;
}

String extractParameter(String *line) {
    if (!line || !*line) {
        if (line && *line)
            *line = NULL;
        return NULL;
    }
    unsigned long long maxSize = 10;
    unsigned char isString = 0;
    unsigned long long i = 0, j = 0;
    String tempLine = strdup(*line);

    String Parameter = (String) calloc(maxSize, sizeof(char));
    for (; *(tempLine + j); i++) {
        if (i == maxSize - 2) {
            maxSize += 10;
            String tempString = (String) calloc(maxSize, sizeof(char));
            for (int k = 0; (*(tempString + k) = *(Parameter + k)); k++);
            free(Parameter);
            Parameter = tempString;
        }
        if (*(tempLine + j) != ',') {
            if (*(tempLine + j) == '\'') {
                if (isString)
                    isString = 0;
                else
                    isString = 1;
            }
            *(Parameter + i) = *(tempLine + j);
        } else {
            j++;
            if (!isString) {
                *(Parameter + i) = '\0';
                strcpy(*line, tempLine + j);
                free(tempLine);
                return Parameter;
            } else {
                *(Parameter + i) = *(tempLine + j);
            }
        }
        j++;
    }
    *(Parameter + i) = '\0';
    strcpy(*line, tempLine + j);
    free(tempLine);
    if (!*Parameter) {
        free(Parameter);
        return NULL;
    }
    return Parameter;
}

String extractMsg(Line_Ptr line) {
    if (line->generalPurposeTokenPtr->tokenType == MSG_TOKEN_TYPE) {
        String tempMsg = strdup(line->generalPurposeTokenPtr->Tokens.MSG_Token->MSG_BODY);
        Msg_String_Ptr stringTree = line->generalPurposeTokenPtr->Tokens.MSG_Token->String_Tree;
        MSG_Register_Ptr registerTree = line->generalPurposeTokenPtr->Tokens.MSG_Token->Registers_Tree;
        String *msgBody = &tempMsg;
        String token = NULL;
        String msgText = NULL;
        String temp_string = NULL;
        if ((!*msgBody || !**msgBody) || (!stringTree && !registerTree)) return "";
        unsigned long long maxSize = 0;
        do {
            token = extractParameter(msgBody);
            if (token && *token == '\'') {
                maxSize += strlen(token);
                if (msgText)
                    temp_string = msgText;
                msgText = (String) calloc(maxSize, sizeof(char));
                if (temp_string)
                    strcpy(msgText, temp_string);
                free(temp_string);
                msgText = strcat(msgText, strReplace(stringTree->message_string, "\\n", "\n"));
                stringTree = stringTree->Next_String;
                free(token);
            } else if (token && *token != '\'') {
                free(token);
                if (registerTree->REGISTER->Data_Type_Settings) {
                    unsigned length = snprintf(NULL, 0, "%lf", registerTree->REGISTER->Register_Values.Dval);
                    token = (String) calloc(length, sizeof(char) * (length + 1));
                    snprintf(token, length + 1, "%lf", registerTree->REGISTER->Register_Values.Dval);
                } else {
                    unsigned length = snprintf(NULL, 0, "%lld", registerTree->REGISTER->Register_Values.Lval);
                    token = (String) calloc(length, sizeof(char) * (length + 1));
                    snprintf(token, length + 1, "%lld", registerTree->REGISTER->Register_Values.Lval);
                }
                maxSize += strlen(token);
                msgText = (String) realloc(msgText, sizeof(char) * maxSize);
                msgText = strcat(msgText, token);
                registerTree = registerTree->NEXT_REGISTER;
                free(token);
            }
        } while (token);
        return msgText;
    } else
        return NULL;
}

String extractLine(String *line) {
    if (!line || !*line || !**line) {
        if (line && *line && !**line) {
            *line = NULL;
        }
        return NULL;
    }
    unsigned long long maxSize = 10;
    unsigned char isString = 0;
    String Parameter = (String) calloc(maxSize, sizeof(char));
    for (unsigned int i = 0; **line; i++) {
        if (i == maxSize - 4) {
            maxSize += 10;
            String tempLine = (String) calloc(maxSize, sizeof(char));
            for (int j = 0; (*(tempLine + j) = *(Parameter + j)); j++);
            free(Parameter);
            Parameter = tempLine;
        }
        if (**line != '\n') {
            if (**line == '\'') {
                if (isString)
                    isString = 0;
                else
                    isString = 1;
            }
            *(Parameter + i) = **line;
        } else {
            if (!isString) {
                (*line)++;
                *(Parameter + i) = '\0';
                return Parameter;
            } else {
                *(Parameter + i++) = '\\';
                *(Parameter + i) = 'n';
            }
        }
        (*line)++;
    }
    *line = NULL;
    if (!*Parameter) {
        free(Parameter);
        return NULL;
    }
    return Parameter;
}

String extractLabel(String_Constant line) {
    if (!line || !*line) {
        return NULL;
    }
    unsigned long long maxSize = 10;
    unsigned long long i = 0, j = 0;
    String Parameter = (String) calloc(maxSize, sizeof(char));
    for (; *(line + j); i++) {
        if (i == maxSize - 2)
            maxSize += 10,
                    Parameter = (String) realloc(Parameter, sizeof(char) * maxSize);
        if (*(line + j) != ':') {
            if (*(line + j) == '\'' || *(line + j) == ' ' || *(line + j) == '\t') {
                free(Parameter);
                return NULL;
            }
            *(Parameter + i) = *(line + j);
        } else {
            j++;
            if (!*(line + j)) {
                *(Parameter + i) = '\0';
                return Parameter;
            }
            free(Parameter);
            return NULL;
        }
        j++;
    }
    *(Parameter + i) = '\0';
    if (!*Parameter) {
        free(Parameter);
        return NULL;
    }
    return Parameter;
}

void printRegister(Register_Ptr reg) {
    if (reg) {
        printf("REGISTER NAME: %s\n", reg->Register_Name);
        printf("REGISTER TYPE: %s\n", Register_TYPES[reg->registerType]);
        printf("REGISTER ADDRESS: %p\n", reg);
        if (reg->Data_Type_Settings)
            printf("REGISTER VALUE (Double): %lf\n",
                   reg->Register_Values.Dval);
        else
            printf("REGISTER VALUE (Long Long): %lld\n",
                   reg->Register_Values.Lval);
    }
}

Register_Ptr searchRegister(Register_Ptr *registerPtr) {
    if (!*registerPtr)
        return *registerPtr;
    Register_Ptr *installationRegister = NULL;
    if (!strcmp((*registerPtr)->Register_Name, "TEMP")) {
        installationRegister = &Temp_Registers_Tree;
        while (*installationRegister) {
            if ((*installationRegister)->Data_Type_Settings == (*registerPtr)->Data_Type_Settings && (
                    (*installationRegister)->Register_Values.Lval == (*registerPtr)->Register_Values.Lval
                    || (*installationRegister)->Register_Values.Dval == (*registerPtr)->Register_Values.Dval
            )) {
                if (*registerPtr != *installationRegister)
                    free((*registerPtr)->Register_Name),
                            free(*registerPtr),
                            *registerPtr = NULL;
                return *installationRegister;
            }
            installationRegister = &(*installationRegister)->Next_Register;
            *installationRegister = *registerPtr;
            return *installationRegister;
        }
    } else {
        installationRegister = &Actual_Registers_Tree;
        while (*installationRegister) {
            if (!strcmp((*installationRegister)->Register_Name, (*registerPtr)->Register_Name)) {
                if (*registerPtr != *installationRegister)
                    free((*registerPtr)->Register_Name),
                            free(*registerPtr),
                            *registerPtr = NULL;
                else
                    (*registerPtr)->registerType = SAVED_REGISTER;
                return *installationRegister;
            }
            installationRegister = &(*installationRegister)->Next_Register;
        }

    }
    installationRegister = &UNDEFINED_REGISTERS;
    while (*installationRegister) {
        if (!strcmp((*installationRegister)->Register_Name, (*registerPtr)->Register_Name)) {
            if (*registerPtr != *installationRegister)
                free((*registerPtr)->Register_Name),
                        free(*registerPtr),
                        *registerPtr = NULL;
            return *installationRegister;
        }
        installationRegister = &(*installationRegister)->Next_Register;
    }
    *installationRegister = *registerPtr;
    (*registerPtr)->Next_Register = NULL;
    return *registerPtr;
}

void printLine(Line_Ptr line) {
    printf("------- INSTRUCTION -------\n");
    printf("LINE CODE: %s\n", line->lineCode ? line->lineCode : "(nil)");
    switch (line->generalPurposeTokenPtr->tokenType) {
        case AU_TOKEN_TYPE:
            printf("INSTRUCTION TYPE: AU INSTRUCTION\n");
            printf("INSTRUCTION SET: %s\n", line->Instruction_String);
            printf("REGISTER A TYPE: %s\n",
                   Register_TYPES[line->generalPurposeTokenPtr->Tokens.Au_Token->RegisterA->registerType]);
            printRegister(line->generalPurposeTokenPtr->Tokens.Au_Token->RegisterA);
            printRegister(line->generalPurposeTokenPtr->Tokens.Au_Token->RegisterB);
            break;
        case JUMP_TOKEN_TYPE:
            printf("INSTRUCTION TYPE: JUMP INSTRUCTION\n");
            printf("INSTRUCTION SET: %s\n", line->Instruction_String);
            printf("ASSOCIATED LABEL NAME: %s\n", line->generalPurposeTokenPtr->Tokens.Jump_Token->Label_Name);
            printf("JUMPING LABEL ADDRESS: %p\n", line->generalPurposeTokenPtr->Tokens.Jump_Token->Line_Address);
            if (line->generalPurposeTokenPtr->Tokens.Jump_Token->Instruction != JMP &&
                line->generalPurposeTokenPtr->Tokens.Jump_Token->CMP_Token) {
                printRegister(line->generalPurposeTokenPtr->Tokens.Jump_Token->CMP_Token->RegisterA);
                printRegister(line->generalPurposeTokenPtr->Tokens.Jump_Token->CMP_Token->RegisterB);
                if (line->generalPurposeTokenPtr->Tokens.Jump_Token->Instruction != JMP)
                    printf("COMPARISON RETURN: %s\n",
                           CMP_RETURNS[line->generalPurposeTokenPtr->Tokens.Jump_Token->CMP_Token->CMP_Val]);
            }
            break;
        case MSG_TOKEN_TYPE:
            printf("INSTRUCTION TYPE: MSG INSTRUCTION\n");
            Msg_String_Ptr tempStringTree = line->generalPurposeTokenPtr->Tokens.MSG_Token->String_Tree;
            MSG_Register_Ptr tempRegisterTree = line->generalPurposeTokenPtr->Tokens.MSG_Token->Registers_Tree;
            printf("MSG BODY: %s\n", line->generalPurposeTokenPtr->Tokens.MSG_Token->MSG_BODY);
            while (tempStringTree) {
                printf("STRING TOKEN: %s\n", tempStringTree->MSG_TOKEN_STRING);
                printf("STRING IN MSG: %s\n", tempStringTree->message_string);
                tempStringTree = tempStringTree->Next_String;
            }
            printf("Registers     {\n");
            while (tempRegisterTree) {
                printRegister(tempRegisterTree->REGISTER);
                tempRegisterTree = tempRegisterTree->NEXT_REGISTER;
            }
            printf("}\n");
            break;
        case RETURN_TOKEN_TYPE:
            printf("INSTRUCTION TYPE: RETURN INSTRUCTION\n");
            printf("INSTRUCTION SET: RET\n");
            printf("ASSOCIATED LABEL NAME: %s\n", line->generalPurposeTokenPtr->Tokens.Return_Token->Label_Name);
            printf("ASSOCIATED LINE ADDRESS: %p\n", line->generalPurposeTokenPtr->Tokens.Return_Token->Line_Address);
            printf("ASSOCIATED CALL TOKEN: %p\n", line->generalPurposeTokenPtr->Tokens.Return_Token->callToken);
            break;
        case END_TOKEN_TYPE:
            printf("INSTRUCTION TYPE: END INSTRUCTION\n");
            break;
        case CALL_TOKEN_TYPE:
            printf("INSTRUCTION TYPE: CALL INSTRUCTION\n");
            printf("INSTRUCTION SET: %s\n", line->Instruction_String);
            printf("ASSOCIATED LABEL NAME: %s\n", line->generalPurposeTokenPtr->Tokens.Call_Token->Label_Name);
            printf("JUMPING LABEL ADDRESS: %p\n", line->generalPurposeTokenPtr->Tokens.Call_Token->Line_Address);
            break;
        case CMP_TOKEN_TYPE:
            printf("INSTRUCTION TYPE: CMP INSTRUCTION\n");
            printf("INSTRUCTION SET: %s\n", line->Instruction_String);
            printRegister(line->generalPurposeTokenPtr->Tokens.CMP_Token->RegisterA);
            printRegister(line->generalPurposeTokenPtr->Tokens.CMP_Token->RegisterB);
            printf("COMPARISON RETURN: %s\n",
                   CMP_RETURNS[line->generalPurposeTokenPtr->Tokens.CMP_Token->CMP_Val]);
            break;
        case LABEL_TOKEN_TYPE:
            printf("INSTRUCTION TYPE: LABEL INSTRUCTION\n");
            printf("INSTRUCTION SET: (%s:)\n", line->generalPurposeTokenPtr->Tokens.Label_Token->Label_Name);
            printf("LABEL NAME: %s\n", line->generalPurposeTokenPtr->Tokens.Label_Token->Label_Name);
            printf("LABEL ADDRESS %p\n", line->generalPurposeTokenPtr->Tokens.Label_Token->Label_Address);
            break;
        default:
            printf("INSTRUCTION TYPE: UNDEFINED INSTRUCTION\n");
            break;
    }
    if (line->Error) {
        printf("ERROR{\n    ERROR TYPE: %s\n", ERRORS_TYPES[line->Error->errorType]);
        printf("    ERROR STRING: %s\n}\n", line->Error->Error_MSG);
    }
    printf("-----------------------\n\n");
}

void printAllStackCode(Line_Ptr line) {
    while (line) {
        printLine(line);
        line = line->Next_Line;
    }
}

unsigned stringTrim(String *lineStringPtr) {
    if (**lineStringPtr) {
        *lineStringPtr = strdup(*lineStringPtr);
        while (**lineStringPtr == ' ' || **lineStringPtr == '\t')
            (*lineStringPtr)++;
        int i = 0;
        for (; *(*lineStringPtr + i); i++);
        for (; *(*lineStringPtr + i - 1) == ' ' || *(*lineStringPtr + i - 1) == '\t'; *(*lineStringPtr + i -
                                                                                        1) = '\0', i--);
        *lineStringPtr = strdup(*lineStringPtr);
        return i;
    } else
        return 0;
}

unsigned beautify(String *code) {
    if (!code || !*code || !**code) {
        if (code && *code && !**code)
            *code = NULL;
        return 0;
    }
#define PRESENT_CHAR *(*code + i)
    if (!*code)
        return 0;
    unsigned i = 0, newCodeCounter = 0, length = 0;
    unsigned long long maxSize = 10;
    unsigned char lastChar = 0, setter = 0;
    String newCode = (String) calloc(maxSize, sizeof(char));
    for (; *(*code + (i > 0 ? i - 1 : 0)) && *(*code + (i > 0 ? i - 1 : 0)) != ';'; i++) {
        if (newCodeCounter == maxSize - 4) {
            maxSize += 10;
            newCode = (String) realloc(newCode, sizeof(char) * (maxSize));
        }
        if (PRESENT_CHAR == '\'') {
            if (setter & (unsigned) 2)
                setter &= (unsigned) 0b11111101;
            else
                setter |= (unsigned) 2;
            if (lastChar)
                *(newCode + newCodeCounter++) = lastChar;
            lastChar = PRESENT_CHAR;
            continue;
        }
        if (!(setter & (unsigned) 2)) {
            if ((!lastChar || lastChar == '\t' || lastChar == ' ' || lastChar == ',') &&
                (PRESENT_CHAR == ' ' || !PRESENT_CHAR || PRESENT_CHAR == ':' ||
                 PRESENT_CHAR == ',' || PRESENT_CHAR == '\t'))
                continue;
            else if (PRESENT_CHAR == '\t') {
                *(newCode + newCodeCounter++) = ' ';
                lastChar = ' ';
                continue;
            } else if (!lastChar && PRESENT_CHAR != ' ' && PRESENT_CHAR != '\t') {
                if (setter & (unsigned) 1)
                    *(newCode + newCodeCounter++) = lastChar;
                setter |= (unsigned) 1;
            } else if (lastChar != ' ' && PRESENT_CHAR != ' ' && PRESENT_CHAR != '\t') {
                *(newCode + newCodeCounter++) = lastChar;
            } else {
                if (setter & (unsigned) 1)
                    *(newCode + newCodeCounter++) = lastChar;
            }
        } else {
            *(newCode + newCodeCounter++) = lastChar;
        }
        lastChar = PRESENT_CHAR;
    }
    *code = realloc(*code, 0);
    *(newCode + newCodeCounter) = '\0';
    *code = strdup(newCode);
    free(newCode);
    return length;
}
