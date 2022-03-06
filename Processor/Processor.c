#include "Processor.h"
#include "../My_Lib/My_Lib.h"

extern const char *BINARY_FILE;

char *Get_Code (int *max_ip)
{
    MY_ASSERT (max_ip, "int *max_ip", NULL_PTR, NULL);
    
    FILE *file_ptr = Open_File (BINARY_FILE, "rb");

    int n_symbs = (int)Define_File_Size (file_ptr);

    char *code = (char *)calloc (n_symbs, sizeof (char));
    MY_ASSERT (code, "unsigned char *code", NE_MEM, NULL);

    fread (code, sizeof (unsigned char), n_symbs, file_ptr);

    Close_File (file_ptr, BINARY_FILE);

    *max_ip = n_symbs;

    return code;
}

#define DEFCMD_(num, name, n_args, code)    \
case num:                                   \
    code;                                   \
    break;

int Process_Code (const char *code_arr, const int max_ip, struct Processor *pentium)
{
    MY_ASSERT (code_arr, "const char *code_arr", NULL_PTR, ERROR);

    for (int ip = 0; ip < max_ip; )
    {
        switch (code_arr[ip])
        {
            #include "../Commands_List.h"

            default: 
            MY_ASSERT (false, "code_arr[ip]", UNDEF_CMD, ERROR);
        }
    }

    return NO_ERRORS;
}
#undef DEFCMD_

double Get_Double (double *num)
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

int Compare_Double (const double first, const double second)
{
    double absolute_value = fabs (first - second);

    if (absolute_value > EPSILON)
        return (first > second) ? GREATER : LESS;
    else
        return EQUAL;
}

int Write_Circle (struct Processor *pentium, const int radius)
{
    MY_ASSERT (pentium,                 "struct *Processor pentium", NULL_PTR, ERROR);
    MY_ASSERT (radius > 1,              "const int radius",          RADIUS,   ERROR);
    MY_ASSERT (radius <= VERT_SIZE / 2, "const int radius",          RADIUS,   ERROR);

    FILE *file = fopen ("./Processor/Circle.txt", "wb");

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

int Change_Deltas (struct Circle *circle, enum Pt_Types mode)
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

int Put_In_RAM (struct Processor *pentium, const struct Circle *circle)
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
