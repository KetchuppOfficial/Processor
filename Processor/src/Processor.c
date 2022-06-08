#include "../include/Processor.h"

// ============================= Constructor and Destructor ============================= //

static const size_t RAM_SIZE  = 4096;
static const size_t GRAM_SIZE = 64000;

struct Processor *Processor_Ctor (void)
{
    struct Processor *proc = (struct Processor *)calloc (1, sizeof (struct Processor));
    MY_ASSERT (proc, "struct Processor *proc", NE_MEM, NULL);

    proc->stack = Stack_Ctor ();
    proc->call_stack = Stack_Ctor ();

    proc->RAM = (double *)calloc (RAM_SIZE, sizeof (double));
    MY_ASSERT (proc->RAM, "proc->RAM", NE_MEM, NULL);

    proc->GRAM = (char *)calloc (GRAM_SIZE, sizeof (char));
    MY_ASSERT (proc->GRAM, "struct Processor *proc", NE_MEM, NULL);

    return proc;
}

int Processor_Dtor (struct Processor *proc)
{
    MY_ASSERT (proc, "struct Processor *proc", NULL_PTR, ERROR);

    Stack_Dtor (proc->stack);
    Stack_Dtor (proc->call_stack);

    free (proc->RAM);
    free (proc->GRAM);

    free (proc);

    return NO_ERRORS;
}
// ====================================================================================== //

// ================================= Processing doubles ================================= //

enum Comparison
{
    LESS = -1,
    EQUAL,
    GREATER
};

static const double EPSILON = 1E-6;

static int Compare_Double (const double first, const double second)
{
    double absolute_value = fabs (first - second);

    if (absolute_value > EPSILON)
        return (first > second) ? GREATER : LESS;
    else
        return EQUAL;
}

static double Get_Double (double *num)
{
    MY_ASSERT (num, "double *num", NULL_PTR, ERROR);

    printf ("Write a number: ");
    for (;;)
    {
        while (scanf ("%lf", num) != 1)
        {
            int symb = 0;
            while ((symb = getchar ()) != '\n') {;}
            printf ("Write an integer number: ");
            printf ("You've written something wrong. Try again\n");
        }
        if (getchar () != '\n')
        {
            printf ("You've written something wrong. Try again\n");
            continue;
        }
        break;
    }

    return NO_ERRORS;
}
// ====================================================================================== //

// =================================== Writing circle =================================== //

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

static const int HOR_SIZE = 320;
static const int VERT_SIZE = 200;

static int Put_In_RAM (struct Processor *pentium, const struct Circle *circle)
{
    MY_ASSERT (pentium, "struct Processor *pentium",   NULL_PTR, ERROR);
    MY_ASSERT (circle,  "const struct Circle *circle", NULL_PTR, ERROR);

    int old_x = circle->this_pt.x;
    int old_y = circle->this_pt.y;

    int new_x_1 =  old_x + circle->R + 1;        // first qadrant
    int new_y_1 = -old_y / 2 + circle->R / 2;

    int new_x_2 =  old_x + circle->R + 1;        // second qadrant
    int new_y_2 =  old_y / 2 + circle->R /2;

    int new_x_3 = -old_x + circle->R + 1;        // third qadrant
    int new_y_3 = -old_y / 2 + circle->R /2;

    int new_x_4 = -old_x + circle->R + 1;        // fourth qadrant
    int new_y_4 =  old_y / 2 + circle->R /2;

    pentium->GRAM[new_x_1 + HOR_SIZE * new_y_1] = 1;
    pentium->GRAM[new_x_2 + HOR_SIZE * new_y_2] = 1;
    pentium->GRAM[new_x_3 + HOR_SIZE * new_y_3] = 1;
    pentium->GRAM[new_x_4 + HOR_SIZE * new_y_4] = 1;

    return NO_ERRORS;
}

static int Change_Deltas (struct Circle *circle, enum Pt_Types mode)
{
    MY_ASSERT (circle, "struct Circle *circle", NULL_PTR, ERROR);

    switch (mode)
    {
        case horizontal:
            circle->Delta += 2 * circle->this_pt.x + 1;
            break;

        case diagonal:
            circle->Delta += 2 * circle->this_pt.x - 2 * circle->this_pt.y + 2;
            break;

        case vertical:
            circle->Delta += -2 * circle->this_pt.y + 1;
            break;

        default: 
            break;
    }
    
    circle->delta_1 = 2 * (circle->Delta + circle->this_pt.y) - 1;
    circle->delta_2 = 2 * (circle->Delta - circle->this_pt.x) - 1;

    return NO_ERRORS;
}

int Write_Circle (struct Processor *pentium, const int radius)
{
    MY_ASSERT (pentium,                 "struct *Processor pentium", NULL_PTR,  ERROR);
    MY_ASSERT (radius > 1,              "const int radius",          UNEXP_VAL, ERROR);
    MY_ASSERT (radius <= VERT_SIZE / 2, "const int radius",          UNEXP_VAL, ERROR);

    FILE *file = fopen ("./Circle.txt", "wb");

    struct Circle circle = 
    {
        {0, radius},
        radius,
        2 * (1 - radius),
        3 - 2 * radius,
        3 - 4 * radius
    };

    Put_In_RAM (pentium, &circle);

    while (circle.this_pt.y > 0)
    {
        if (circle.Delta < 0)
        {
            circle.this_pt.x++;

            if (circle.delta_1 <= 0)
                Change_Deltas (&circle, horizontal);
            else
            {
                circle.this_pt.y--;
                Change_Deltas (&circle, diagonal);
            }
        }
        else if (circle.Delta > 0)
        {
            circle.this_pt.y--;

            if (circle.delta_2 <= 0)
            {
                circle.this_pt.x++;
                Change_Deltas (&circle, diagonal);
            }
            else
                Change_Deltas (&circle, vertical);
        }
        else
        {
            circle.this_pt.x++;
            circle.this_pt.y--;
            Change_Deltas (&circle, diagonal);
        }

        Put_In_RAM (pentium, &circle);
    }

    for (int i = 0; i < VERT_SIZE; i++)
    {
        for (int j = 0; j < HOR_SIZE; j++)
        {
            if (pentium->GRAM[i * HOR_SIZE + j] == 1)
                fprintf (file, "+");
            else
                fprintf (file, " ");
        }

        fprintf (file, "\n");
    }

    fclose (file);

    return NO_ERRORS;
}
// ====================================================================================== //

// =================================== Code execution =================================== //

#define PUSH(value)   Stack_Push (proc->stack,      value);
#define PUSH_F(value) Stack_Push (proc->call_stack, value);
#define POP(addr)     Stack_Pop  (proc->stack,      addr);
#define POP_F(addr)   Stack_Pop  (proc->call_stack, addr);

//*****************************************************************************************************
#define RAM_REG_NUM code_arr[ip] == 1 && code_arr[ip + 1] != 0 && code_arr[ip + 2] == 1

#define PUSH_RAM_REG_NUM                                                                            \
do                                                                                                  \
{                                                                                                   \
    PUSH (proc->RAM[(int)(proc->REG[code_arr[ip + 1] - 1] + *(int *)(code_arr + ip + 3))]);         \
    ip += 3 + sizeof (int);                                                                         \
}                                                                                                   \
while (0)

#define POP_RAM_REG_NUM                                                                             \
do                                                                                                  \
{                                                                                                   \
    POP (&(proc->RAM[(int)(proc->REG[code_arr[ip + 1] - 1] + *(int *)(code_arr + ip + 3))]));       \
    ip += 3 + sizeof (int);                                                                         \
}                                                                                                   \
while (0)
//*****************************************************************************************************

//*****************************************************************************************************
#define RAM_REG code_arr[ip] == 1 && code_arr[ip + 1] != 0 && code_arr[ip + 2] == 0

#define PUSH_RAM_REG                                                    \
do                                                                      \
{                                                                       \
    PUSH (proc->RAM[(int)(proc->REG[code_arr[ip + 1] - 1])]);           \
    ip += 3;                                                            \
}                                                                       \
while (0)

#define POP_RAM_REG                                                     \
do                                                                      \
{                                                                       \
    POP (&(proc->RAM[(int)(proc->REG[code_arr[ip + 1] - 1])]));         \
    ip += 3;                                                            \
}                                                                       \
while (0)
//*****************************************************************************************************

//*****************************************************************************************************
#define RAM_NUM code_arr[ip] == 1 && code_arr[ip + 1] == 0 && code_arr[ip + 2] == 1

#define PUSH_RAM_NUM                                                        \
do                                                                          \
{                                                                           \
    PUSH (proc->RAM[(int)(proc->REG[*(int *)(code_arr + ip + 3)])]);        \
    ip += 3 + sizeof (int);                                                 \
}                                                                           \
while (0)

#define POP_RAM_NUM                                                             \
do                                                                              \
{                                                                               \
    POP (&(proc->RAM[(int)(proc->REG[*(int *)(code_arr + ip + 3)])]));          \
    ip += 3 + sizeof (int);                                                     \
}                                                                               \
while (0)
//*****************************************************************************************************

//*****************************************************************************************************
#define REG_ code_arr[ip] == 0 && code_arr[ip + 1] != 0 && code_arr[ip + 2] == 0

#define PUSH_REG                                    \
do                                                  \
{                                                   \
    PUSH (proc->REG[code_arr[ip + 1] - 1]);         \
    ip += 3;                                        \
}                                                   \
while (0)

#define POP_REG                                     \
do                                                  \
{                                                   \
    POP (&(proc->REG[code_arr[ip + 1] - 1]));       \
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

#define Arithmetic(operator)                                                \
do                                                                          \
{                                                                           \
    long stack_size = Get_Stack_Size (proc->stack);                         \
    MY_ASSERT (stack_size >= 2, "stack size", UNEXP_VAL, ERROR);            \
                                                                            \
    double a = 0.0, b = 0.0;                                                \
    POP(&b);                                                                \
    POP(&a);                                                                \
                                                                            \
    switch (operator)                                                       \
    {                                                                       \
        case '+':   PUSH (a + b); break;                                    \
        case '-':   PUSH (a - b); break;                                    \
        case '*':   PUSH (a * b); break;                                    \
        case '/':                                                           \
        {                                                                   \
            if (Compare_Double (a, 0) != EQUAL)                             \
            {                                                               \
                PUSH (a / b); break;                                        \
            }                                                               \
            else                                                            \
            {                                                               \
                MY_ASSERT (false, "double a", ZERO_DIV, ERROR);             \
            }                                                               \
            break;                                                          \
        }                                                                   \
        default: MY_ASSERT (false, "None", UNDEF_BEH, ERROR);               \
    }                                                                       \
    ip++;                                                                   \
}                                                                           \
while (0)

#define JUMP(condition)                                                                 \
do                                                                                      \
{                                                                                       \
    long stack_size = Get_Stack_Size (proc->stack);                                     \
    MY_ASSERT (stack_size >= 2, "stack size", UNEXP_VAL, ERROR);                        \
                                                                                        \
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

#define DEFCMD_(num, name, n_args, code)    \
case num:                                   \
    code;                                   \
    break;

int Process_Code (const char *code_arr, const int max_ip, struct Processor *proc)
{
    MY_ASSERT (code_arr, "const char *code_arr",   NULL_PTR, ERROR);
    MY_ASSERT (proc,     "struct Processor *proc", NULL_PTR, ERROR);

    for (int ip = 0; ip < max_ip; )
    {
        switch (code_arr[ip])
        {
            #include "../Commands_List.h"

            default: 
            MY_ASSERT (false, "code_arr[ip]", UNEXP_SYMB, ERROR);
        }
    }

    return NO_ERRORS;
}
#undef DEFCMD_
// ====================================================================================== //
