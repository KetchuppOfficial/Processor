#ifndef PROCESSOR_H_INCLUDED
#define PROCESSOR_H_INCLUDED

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "./Stack/Stack.h"

struct Processor
{
    struct Stack stack;
    struct Stack call_stack;
    double RAM[4096];
    double REG[4];
    char   GRAM[64000];
};

enum Comparison
{
    LESS = -1,
    EQUAL,
    GREATER
};

struct Point 
{
    int x;
    int y;
};

struct Circle
{
    struct Point this_pt;
    int R;
    int Delta;
    int delta_1;
    int delta_2;
};

enum Pt_Types
{
    horizontal,
    diagonal,
    vertical
};

const double EPSILON = 1E-6;
const int HOR_SIZE = 320;
const int VERT_SIZE = 200;

char   *Get_Code      (int *max_ip);
int    Process_Code   (const char *code_arr, const int max_ip, struct Processor *pentium);
double Get_Double     (double *num);
int    Compare_Double (const double first, const double second);
int    Write_Circle   (struct Processor *pentium, const int radius);
int    Change_Deltas  (struct Circle *circle, Pt_Types mode);
int    Put_In_RAM     (struct Processor *pentium, const struct Circle *circle);

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
            if (Compare_Double (a, 0) != EQUAL)                 \
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

#define JUMP(condition)                                                                 \
do                                                                                      \
{                                                                                       \
    double first = 0, second = 0;                                                       \
                                                                                        \
    POP (&second);                                                                      \
    POP (&first);                                                                       \
                                                                                        \
    int comp_res = Compare_Double (first, second);                                      \
                                                                                        \
    if ((!strcmp (condition, ">=") && (comp_res == EQUAL  || comp_res == GREATER)) ||   \
        (!strcmp (condition, ">")  &&  comp_res == GREATER                       ) ||   \
        (!strcmp (condition, "<=") && (comp_res == EQUAL  || comp_res == LESS)   ) ||   \
        (!strcmp (condition, "<")  &&  comp_res == LESS                          ) ||   \
        (!strcmp (condition, "==") &&  comp_res == EQUAL                         ) ||   \
        (!strcmp (condition, "!=") &&  comp_res != EQUAL                         )   )  \
    {                                                                                   \
        ip = *(int *)(code_arr + ip + 1);                                               \
    }                                                                                   \
    else                                                                                \
        ip += 1 + sizeof (int);                                                         \
}                                                                                       \
while (0)

#endif