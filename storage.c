#include "Requirements.h"

unsigned long long Lines_Counter = 0;
unsigned long long Error_Counter = 0;
CMP_Token_Ptr LAST_CMP = NULL;
Jump_Token_Ptr TEMP_JUMPS_CONTAINER = NULL;
Register_Ptr Temp_Registers_Tree = NULL;
Register_Ptr Actual_Registers_Tree = NULL;
Register_Ptr UNDEFINED_REGISTERS = NULL;
Register_Ptr *LAST_FREE_REGISTER_IN_TEMP_REGISTER_TREE = &Temp_Registers_Tree;
Register_Ptr *LAST_FREE_REGISTER_IN_ACTUAL_REGISTER_TREE = &Actual_Registers_Tree;
Line_Ptr Actual_Lines_tree = NULL;
Label_Token_Ptr Labels_Container = NULL;
Label_Token_Ptr *LAST_FREE_Labels_Container = &Labels_Container;
Calls_Tree_Ptr TEMP_CALLS_CONTAINER = NULL;
Call_Token_Ptr Bounded_Calls_Container = NULL;
Call_Token_Ptr *LAST_BOUNDED_CALL_CONTAINER = &Bounded_Calls_Container;
Label_Token_Ptr Last_LABEL = NULL;
