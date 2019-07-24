#ifndef STORAGE
#define STORAGE
extern unsigned long long Lines_Counter;
extern unsigned long long Error_Counter;
extern CMP_Token_Ptr LAST_CMP;
extern Jump_Token_Ptr TEMP_JUMPS_CONTAINER;
extern Register_Ptr Temp_Registers_Tree;
extern Register_Ptr Actual_Registers_Tree;
extern Register_Ptr *LAST_FREE_REGISTER_IN_TEMP_REGISTER_TREE;
extern Register_Ptr *LAST_FREE_REGISTER_IN_ACTUAL_REGISTER_TREE;
extern Line_Ptr Actual_Lines_tree;
extern Label_Token_Ptr Labels_Container;
extern Label_Token_Ptr *LAST_FREE_Labels_Container;
extern Calls_Tree_Ptr TEMP_CALLS_CONTAINER;
extern Call_Token_Ptr Bounded_Calls_Container;
extern Call_Token_Ptr *LAST_BOUNDED_CALL_CONTAINER;
extern Label_Token_Ptr Last_LABEL;
extern Register_Ptr UNDEFINED_REGISTERS;
#endif
