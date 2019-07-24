#ifndef TOKENS
#define TOKENS
typedef char *String;
typedef char const *const String_Constant;
typedef struct MSG_REGISTER *MSG_Register_Ptr;
typedef struct Register *Register_Ptr;
typedef struct Au_Token *Au_Token_Ptr;
typedef struct Label_Token *Label_Token_Ptr;
typedef struct Jump_Token *Jump_Token_Ptr;
typedef struct MSG_STRING *Msg_String_Ptr;
typedef struct Msg_Token *Msg_Token_Ptr;
typedef struct CMP_Token *CMP_Token_Ptr;
typedef struct GENERAL_PURPOSE_TOKEN *GENERAL_PURPOSE_TOKEN_PTR;
typedef struct Line *Line_Ptr;
typedef struct Error *Error_Ptr;
typedef struct Call_Token *Call_Token_Ptr;
typedef struct Ret_Token *Return_Token_Ptr;
typedef struct Calls_Tree *Calls_Tree_Ptr;

typedef enum Token_Type {
    AU_TOKEN_TYPE, JUMP_TOKEN_TYPE, LABEL_TOKEN_TYPE,
    CALL_TOKEN_TYPE, MSG_TOKEN_TYPE, RETURN_TOKEN_TYPE,
    END_TOKEN_TYPE, CMP_TOKEN_TYPE, UNIDENTIFIED_TOKEN_TYPE
} Token_Type;
typedef enum Error_Type {
    SYNTAX_ERROR,
    NAMING_ERROR,
    NULL_REGISTER_ERROR,
    PARAMETRIZATION_ERROR,
    UNBOUNDED_JUMP_CONDITION,
    UNBOUNDED_CALL_CONDITION,
    INVALID_RETURN_TOKEN_ERROR,
    DUPLICATED_LABEL_ERROR,
    REGISTER_TYPE_ERROR,
    UNDEFINED_REGISTER_NAME,
    UNBOUNDED_CMP_ERROR
} Error_Type;
typedef enum Au_Instruction {
    MOV, ADD, SUB, DIV, MUL, INC, DEC
} Au_Instructions;
typedef enum Register_Type {
    SAVED_REGISTER,
    UNDEFINED_REGISTER,
    TEMP_REGISTER
} Register_Type;
typedef enum Jump_Instruction {
    JMP, JNE, JE, JGE, JG, JLE, JL
} Jump_Instruction;
typedef enum Cmp_Instruction {
    EQUAL,
    GREATER_THAN,
    LESS_THAN
} Cmp_Instruction;

typedef struct Register {
    String Register_Name;
    Register_Type registerType;
    char Data_Type_Settings; /*To check if it's double (signed or unsigned)
                              * or it's long long signed or unsigned long long too)
                              */
    union Register_Values {
        double Dval;
        long long Lval;
    } Register_Values;
    Register_Ptr Next_Register;
} Register;
typedef struct Au_Token {
    Au_Instructions Instruction;
    Register_Ptr RegisterA;
    Register_Ptr RegisterB;
} Au_Token;
typedef struct CMP_Token {
    Register_Ptr RegisterA;
    Register_Ptr RegisterB;
    Cmp_Instruction CMP_Val;
} CMP_Token;
typedef struct Jump_Token {
    Jump_Instruction Instruction;
    Line_Ptr Line_Address;
    String Label_Name;
    CMP_Token_Ptr CMP_Token;
    Jump_Token_Ptr Next_Jump;
} Jump_Token;
typedef struct Line {
    String lineCode;
    String Instruction_String;
    GENERAL_PURPOSE_TOKEN_PTR generalPurposeTokenPtr;
    Error_Ptr Error;
    Line_Ptr Next_Line;
} Line;
typedef struct Label_Token {
    String Label_Name;
    Line_Ptr Label_Address;
    Label_Token_Ptr Next_Label;
} Label_Token;
typedef struct MSG_REGISTER {
    Register_Ptr REGISTER;
    MSG_Register_Ptr NEXT_REGISTER;
} MSG_REGISTER;
typedef struct MSG_STRING {
    String message_string;
    String MSG_TOKEN_STRING;
    Msg_String_Ptr Next_String;
} MSG_STRING;
typedef struct Msg_Token {
    String MSG_BODY;
    Msg_String_Ptr String_Tree;
    MSG_Register_Ptr Registers_Tree;
} Msg_Token;
typedef struct Error {
    String Error_MSG;
    Error_Type errorType;
} Error;
typedef struct Call_Token {
    unsigned char isCalling;
    Line_Ptr associatedLine;
    Line_Ptr Line_Address;
    String Label_Name;
    Call_Token_Ptr Next_Call;
} Call_Token;
typedef struct Calls_Tree {
    Call_Token_Ptr call;
    Calls_Tree_Ptr Next_Node;
} Calls_Tree;
typedef struct Ret_Token {
    String Label_Name;
    Line_Ptr Line_Address;
    Call_Token_Ptr callToken;
} Ret_Token;
typedef struct GENERAL_PURPOSE_TOKEN {
    Token_Type tokenType;
    union Tokens {
        Au_Token_Ptr Au_Token;
        Jump_Token_Ptr Jump_Token;
        Label_Token_Ptr Label_Token;
        Msg_Token_Ptr MSG_Token;
        CMP_Token_Ptr CMP_Token;
        Call_Token_Ptr Call_Token;
        Return_Token_Ptr Return_Token;
    } Tokens;
} GENERAL_PURPOSE_TOKEN;
#endif
