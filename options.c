#include "options.h"

static String_Constant availableOptions[] = {"rF"};

String readCodeFromFile(String_Constant filePath) {
    FILE *codeFilePtr = fopen(filePath, "r");
    if (codeFilePtr != NULL) {
        unsigned long long maxSize = 10, codeCounter = 0;
        char c = 0;
        String code = (String) calloc(maxSize, sizeof(char));
        while ((c = (char) getc(codeFilePtr)) != EOF) {
            if (codeCounter == maxSize - 2) {
                maxSize += 10;
                String tempCode = (String) calloc(maxSize, sizeof(char));
                for (unsigned long long j = 0; j < codeCounter; j++)
                    *(tempCode + j) = *(code + j);
                free(code);
                code = tempCode;
            }
            *(code + codeCounter++) = c;
        }
        fclose(codeFilePtr);
        return code;
    } else {
        fprintf(stderr, "File : %s is not found\n", filePath);
        exit(-1);
    }
}

