#include "Requirements.h"

void parse(void);

void registerNameChecker(Line_Ptr line, Register_Ptr *registerPtr);

void lineParamChecker(Line_Ptr line);

void labelNameChecker(Line_Ptr line);

void checkLabelDuplications(Line_Ptr line);

unsigned paramsLens(String code);

Error_Ptr makeParametersTypesError(String lineCode);

Error_Ptr makeRegisterNamingError(String lineCode, String name);

Error_Ptr makeLabelNameError(String lineCode, String name);

Error_Ptr makeUndefinedRegisterError(String lineCode, String name);

Error_Ptr makeNullRegisterError(String lineCode);

Error_Ptr makeParamError(String lineCode);

Error_Ptr makeSyntaxError(String lineCode);

Error_Ptr makeDuplicateLabelError(String lineCode);

Error_Ptr makeUnboundedCallError(String lineCode);

Error_Ptr makeUnboundedJumpError(String lineCode);

Error_Ptr makeInvalidReturnError(String lineCode);

Error_Ptr makeUnboundedReturnError(String lineCode);

Error_Ptr makeCallWithoutReturn(String lineCode);

Error_Ptr makeUnboundedCmpError(String lineCode);

Error_Ptr makeRegisterTypeError(String lineCode, Register_Type type);
