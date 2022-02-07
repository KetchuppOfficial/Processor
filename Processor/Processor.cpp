#include "Processor.h"
#include "../Reading_File.h"
#include "../Log_File.h"

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