#ifndef REQUIREMENTS
#define REQUIREMENTS

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

String strReplace(String string, String substr, String replacement);

String extractCmd(String *line);

String extractParameter(String *line);

String extractMsg(Line_Ptr line);

String extractLine(String *line);

void printLine(Line_Ptr line);

void printAllStackCode(Line_Ptr line);

unsigned stringTrim(String *lineStringPtr);

unsigned beautify(String *code);

#endif
