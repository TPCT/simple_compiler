#ifndef REQUIREMENTS

#include "Requirements.h"

#endif

Register_Ptr extractRegister(String *code);

Line_Ptr addLine(Line_Ptr *, Line_Ptr *);

void codeReader(String code);

void lineMaker(String *token);

void tokenize(String *lineStringPtr, Line_Ptr line, GENERAL_PURPOSE_TOKEN_PTR gptPtr);

void assignLabelsToJumps(void);

void assignLabelsToCalls(void);

void tempRegisterPtrAdd(Register_Ptr *savingRegister, Register_Ptr *registerPtr);

void actualRegisterPtrAdd(Register_Ptr *savingRegister, Register_Ptr *registerPtr);

void addLabel(Label_Token_Ptr *label);

void addJump(Jump_Token_Ptr *savingPtr, Jump_Token_Ptr *jumpPtr);

void addCall(Call_Token_Ptr *savingPtr, Call_Token_Ptr *callPtr);

void makeCallToken(Line_Ptr line, String *lineStringPtr);

void makeCmpToken(Line_Ptr line, String *lineStringPtr);

void makeMsgToken(Line_Ptr line, String *Line, String token);

void makeAuToken(Line_Ptr line, String *Line, Au_Instructions i);

void makeJumpToken(Line_Ptr line, String *Line, Jump_Instruction i);

void makeLabelToken(Line_Ptr line);

void makeRegister(Register_Ptr *regPtr, String *code);

void makeUndefinedToken(Line_Ptr line);

void makeReturnToken(Line_Ptr line);

void specialMakeRegister(Register_Ptr *regPtr, String *code);

