#ifndef REQUIREMENTS
#define REQUIREMENTS
typedef char *String;
typedef const char *const String_Constant;
#include <string.h>
#include <malloc.h>
#include <ctype.h>
#include <stdlib.h>
#include <stdio.h>
#include "tokens.h"
#include "storage.h"

extern String_Constant Register_TYPES[];
extern String_Constant ERRORS_TYPES[];

Register_Ptr searchRegister(Register_Ptr *registerPtr);

String strReplace(String_Constant string, String_Constant substr, String_Constant replacement);


String extractLabel(String_Constant line);

String extractCmd(String *line, unsigned char *isLabel);

String extractParameter(String *line);

String extractMsg(Line_Ptr line);

String extractLine(String line);

void printRegister(Register_Ptr reg);
void printAllStackCode(Line_Ptr line);

void printLine(Line_Ptr line);

unsigned beautify(String *code);

#endif
