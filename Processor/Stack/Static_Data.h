#ifndef STATIC_DATA_H_INCLUDED
#define STATIC_DATA_H_INCLUDED

//-----------------------------------------------
//Defining error reports
static const char *NULL_STACK    = "Pointer on the stack is NULL";
static const char *UNINIT_STACK  = "It's forbidden to use uninitialized stack";
static const char *NE_MEM        = "Impossible to allocate enough memory";
static const char *ZERO_POP      = "It's forbidden to pop stack which size is 0";
static const char *NULL_ELEM_PTR = "Pointer on the variable to save popped value is NULL";
static const char *MULT_CTOR     = "It's forbidden to initialize stack more than once";
static const char *BAD_RESIZE    = "It's forbidden to resize down stack which capacity is minimal or less than minimal";
static const char *RSZ_ERR       = "Resizing error";
static const char *NULL_FILE     = "Pointer on the file is NULL";
static const char *L_S_CANARY_CG = "Left stack canary changed its value";
static const char *R_S_CANARY_CG = "Right stack canary changed its value";
static const char *L_D_CANARY_CG = "Left data canary changed its value";
static const char *R_D_CANARY_CG = "Right data canary changed its value";
static const char *NULL_PTR      = "Pointer is NULL";
//---------------------------------------

#endif // STATIC_DATA_H_INCLUDED
