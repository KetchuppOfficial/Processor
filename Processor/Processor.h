#ifndef PROCESSOR_H_INCLUDED
#define PROCESSOR_H_INCLUDED

#include <stdio.h>
#include <stdlib.h>

struct Processor
{
    struct Stack stack;
    struct Stack call_stack;
    double RAM[4096];
    double Registers[4];
};

char *Get_Code (int *max_ip);
int Process_Code (const char *code_arr, const int max_ip);

#endif