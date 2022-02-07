#ifndef PROCESSOR_H_INCLUDED
#define PROCESSOR_H_INCLUDED

#include <stdio.h>
#include <stdlib.h>
#include "./Stack/Stack.h"

struct Processor
{
    struct Stack stack;
    struct Stack call_stack;
    double RAM[4096];
    double REG[4];
};

char *Get_Code (int *max_ip);
int Process_Code (const char *code_arr, const int max_ip, struct Processor *pentium);
double Get_Double (double *num);

#define PUSH(value)   Stack_Push (&(pentium->stack), value);
#define PUSH_F(value) Stack_Push (&(pentium->call_stack), value);
#define POP(addr)     Stack_Pop  (&(pentium->stack), addr);
#define POP_F(addr)   Stack_Pop  (&(pentium->call_stack), addr);

//*****************************************************************************************************
#define RAM_REG_NUM code_arr[ip] == 1 && code_arr[ip + 1] != 0 && code_arr[ip + 2] == 1

#define PUSH_RAM_REG_NUM                                                                            \
do                                                                                                  \
{                                                                                                   \
    PUSH (pentium->RAM[(int)(pentium->REG[code_arr[ip + 1] - 1] + *(int *)(code_arr + ip + 3))]);   \
    ip += 3 + sizeof (int);                                                                         \
}                                                                                                   \
while (0)

#define POP_RAM_REG_NUM                                                                             \
do                                                                                                  \
{                                                                                                   \
    POP (&(pentium->RAM[(int)(pentium->REG[code_arr[ip + 1] - 1] + *(int *)(code_arr + ip + 3))])); \
    ip += 3 + sizeof (int);                                                                         \
}                                                                                                   \
while (0)
//*****************************************************************************************************

//*****************************************************************************************************
#define RAM_REG code_arr[ip] == 1 && code_arr[ip + 1] != 0 && code_arr[ip + 2] == 0

#define PUSH_RAM_REG                                                    \
do                                                                      \
{                                                                       \
    PUSH (pentium->RAM[(int)(pentium->REG[code_arr[ip + 1] - 1])]);     \
    ip += 3;                                                            \
}                                                                       \
while (0)

#define POP_RAM_REG                                                     \
do                                                                      \
{                                                                       \
    POP (&(pentium->RAM[(int)(pentium->REG[code_arr[ip + 1] - 1])]));   \
    ip += 3;                                                            \
}                                                                       \
while (0)
//*****************************************************************************************************

//*****************************************************************************************************
#define RAM_NUM code_arr[ip] == 1 && code_arr[ip + 1] == 0 && code_arr[ip + 2] == 1

#define PUSH_RAM_NUM                                                        \
do                                                                          \
{                                                                           \
    PUSH (pentium->RAM[(int)(pentium->REG[*(int *)(code_arr + ip + 3)])]);  \
    ip += 3 + sizeof (int);                                                 \
}                                                                           \
while (0)

#define POP_RAM_NUM                                                             \
do                                                                              \
{                                                                               \
    POP (&(pentium->RAM[(int)(pentium->REG[*(int *)(code_arr + ip + 3)])]));    \
    ip += 3 + sizeof (int);                                                     \
}                                                                               \
while (0)
//*****************************************************************************************************

//*****************************************************************************************************
#define REG_ code_arr[ip] == 0 && code_arr[ip + 1] != 0 && code_arr[ip + 2] == 0

#define PUSH_REG                                    \
do                                                  \
{                                                   \
    PUSH (pentium->REG[code_arr[ip + 1] - 1]);      \
    ip += 3;                                        \
}                                                   \
while (0)

#define POP_REG                                     \
do                                                  \
{                                                   \
    POP (&(pentium->REG[code_arr[ip + 1] - 1]));    \
    ip += 3;                                        \
}                                                   \
while (0)
//*****************************************************************************************************

//*****************************************************************************************************
#define NUM_ code_arr[ip] == 0 && code_arr[ip + 1] == 0 && code_arr[ip + 2] == 1

#define PUSH_NUM                            \
do                                          \
{                                           \
    PUSH (*(double *)(code_arr + ip + 3));  \
    ip += 3 + sizeof (double);              \
}                                           \
while (0)
//*****************************************************************************************************

#define NOTHING code_arr[ip] == 0 && code_arr[ip + 1] == 0 && code_arr[ip + 2] == 0

#define Arithmetic(operator)                                    \
do                                                              \
{                                                               \
    double a = 0.0, b = 0.0;                                    \
    POP(&b);                                                    \
    POP(&a);                                                    \
                                                                \
    switch (operator)                                           \
    {                                                           \
        case '+':   PUSH (a + b); break;                        \
        case '-':   PUSH (a - b); break;                        \
        case '*':   PUSH (a * b); break;                        \
        case '/':                                               \
        {                                                       \
            if (a)                                              \
            {                                                   \
                PUSH (a / b); break;                            \
            }                                                   \
            else                                                \
            {                                                   \
                MY_ASSERT (false, "double a", ZERO_DIV, ERROR); \
            }                                                   \
            break;                                              \
        }                                                       \
        default: MY_ASSERT (false, "None", UNDEF_BEH, ERROR);   \
    }                                                           \
    ip++;                                                       \
}                                                               \
while (0)

#define JUMP(condition)             \
do                                  \
{                                   \
    double first = 0, second = 0;   \
                                    \
    POP (&second);                  \
    POP (&first);                   \
                                    \
    if (first condition second)     \
        ip = code_arr[ip + 1];      \
    else                            \
        ip += 2;                    \
}                                   \
while (0)

#endif