#ifndef PROCESSOR_H_INCLUDED
#define PROCESSOR_H_INCLUDED

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "Stack.h"
#include "../../My_Lib/My_Lib.h"

#ifndef DEBUG
#undef MY_ASSERT
#define MY_ASSERT(condition, var, err_num, error) ;
#endif

struct Processor
{
    struct Stack *stack;
    struct Stack *call_stack;
    double *RAM;
    char   *GRAM;
    double REG[4];
};

struct Processor *Processor_Ctor (void);
int               Processor_Dtor (struct Processor *proc);
int               Process_Code   (const char *code_arr, const int max_ip, struct Processor *proc);

#endif
