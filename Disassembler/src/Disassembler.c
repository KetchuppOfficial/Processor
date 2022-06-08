#include "../Disassembler.h"

//*************************************************************************
#define DEFCMD_(num, name, n_args, code)    \
case num:                                   \
        return #name;

static const char *Get_CMD_Name (const int cmd_n)
{
    switch (cmd_n)
    {
        #include "../../Commands_List.h"

        default:
            MY_ASSERT (false, "const int cmd_n", UNEXP_VAL, NULL);
            break;
    }

    return NULL;
}
#undef DEFCMD_

static int Check_If_Jump (const int cmd_num)
{
    const char *cmd_name = Get_CMD_Name (cmd_num);

    if (strncmp ("jmp",  cmd_name, sizeof ("jmp")  - 1) == 0 ||
        strncmp ("jae",  cmd_name, sizeof ("jae")  - 1) == 0 ||
        strncmp ("jbe",  cmd_name, sizeof ("jbe")  - 1) == 0 ||
        strncmp ("ja",   cmd_name, sizeof ("ja")   - 1) == 0 ||
        strncmp ("jb",   cmd_name, sizeof ("jb")   - 1) == 0 ||
        strncmp ("jne",  cmd_name, sizeof ("jne")  - 1) == 0 ||
        strncmp ("je",   cmd_name, sizeof ("je")   - 1) == 0 ||
        strncmp ("call", cmd_name, sizeof ("call") - 1) == 0)
        return 1;
    else
        return 0;
}

static int Check_If_Push_Pop (const int cmd_num)
{
    const char *cmd_name = Get_CMD_Name (cmd_num);

    if (strncmp ("push", cmd_name, sizeof ("push") - 1) == 0 ||
        strncmp ("pop",  cmd_name, sizeof ("pop")  - 1) == 0)
        return 1;
    else
        return 0;
}
//*************************************************************************
#define DEFCMD_(num, name, n_args, code)    \
case num:                                   \
    return n_args;

static int Check_N_Args (const int cmd_n)
{       
    switch (cmd_n)
    {
        #include "../../Commands_List.h"

        default:
            MY_ASSERT (false, "const int cmd_n", UNEXP_VAL, ERROR);
            break;
    }
}
#undef DEFCMD_
//*************************************************************************

#define DEFCMD_(num, name, n_args, code)                                            \
case num:                                                                           \
    if (Check_N_Args (num) == 0)                                                    \
        ip++;                                                                       \
    else if (Check_If_Push_Pop (num) == 1)                                          \
    {                                                                               \
        ip++;                                                                       \
        if (code_arr[ip] == 1 && code_arr[ip + 1] != 0 && code_arr[ip + 2] == 1)    \
            ip += (3 + sizeof (int));                                               \
        if (code_arr[ip] == 1 && code_arr[ip + 1] != 0 && code_arr[ip + 2] == 0)    \
            ip += 3;                                                                \
        if (code_arr[ip] == 1 && code_arr[ip + 1] == 0 && code_arr[ip + 2] == 1)    \
            ip += (3 + sizeof (int));                                               \
        if (code_arr[ip] == 0 && code_arr[ip + 1] != 0 && code_arr[ip + 2] == 0)    \
            ip += 3;                                                                \
        if (code_arr[ip] == 0 && code_arr[ip + 1] == 0 && code_arr[ip + 2] == 1)    \
            ip += (3 + sizeof (double));                                            \
        if (code_arr[ip] == 0 && code_arr[ip + 1] == 0 && code_arr[ip + 2] == 0)    \
            ip += 3;                                                                \
    }                                                                               \
    else if (Check_If_Jump (num) == 1)                                              \
    {                                                                               \
        ip++;                                                                       \
        label_arr[label_i++] = *(int *)(code_arr + ip);                             \
        ip += sizeof (int);                                                         \
    }                                                                               \
    else                                                                            \
        ip += 1 + sizeof (double);                                                  \
                                                                                    \
    break;                                                              

static char *First_Passing (const char *code_arr, const int max_ip, int *n_labels)
{
    MY_ASSERT (code_arr, "const char *code_arr", NULL_PTR, NULL);

    char *label_arr = (char *)calloc (max_ip, sizeof (char));
    MY_ASSERT (label_arr, "char *label_arr", NE_MEM, NULL);
    int label_i = 0;

    for (int ip = 0; ip < max_ip; )
    {
        switch (code_arr[ip])
        {
            #include "../Commands_List.h"

            default: 
            MY_ASSERT (false, "code_arr[ip]", UNEXP_SYMB, NULL);
        }
    }

    *n_labels = label_i;

    return label_arr;
}
#undef DEFCMD_
//*************************************************************************

#define DEFCMD_(num, name, n_args, code)                                                    \
case num:                                                                                   \
    if (Check_N_Args (num) == 0)                                                            \
    {                                                                                       \
        fprintf (file_ptr, "%s\n", Get_CMD_Name (num));                                     \
        ip++;                                                                               \
    }                                                                                       \
    else if (Check_If_Push_Pop (num) == 1)                                                  \
    {                                                                                       \
        ip++;                                                                               \
        if (code_arr[ip] == 1 && code_arr[ip + 1] != 0 && code_arr[ip + 2] == 1)            \
        {                                                                                   \
            fprintf (file_ptr, "%s [%cx + %d]\n",                                           \
                               Get_CMD_Name (num),                                          \
                               code_arr[ip + 1] + 'a',                                      \
                               *(int *)(code_arr + ip + 3));                                \
            ip += 3 + sizeof (int);                                                         \
        }                                                                                   \
        if (code_arr[ip] == 1 && code_arr[ip + 1] != 0 && code_arr[ip + 2] == 0)            \
        {                                                                                   \
            fprintf (file_ptr, "%s [%cx]\n",                                                \
                               Get_CMD_Name (num),                                          \
                               code_arr[ip + 1] + 'a');                                     \
            ip += 3;                                                                        \
        }                                                                                   \
        if (code_arr[ip] == 1 && code_arr[ip + 1] == 0 && code_arr[ip + 2] == 1)            \
        {                                                                                   \
            fprintf (file_ptr, "%s [%d]\n",                                                 \
                               Get_CMD_Name (num),                                          \
                               *(int *)(code_arr + ip + 3));                                \
            ip += 3 + sizeof (int);                                                         \
        }                                                                                   \
        if (code_arr[ip] == 0 && code_arr[ip + 1] != 0 && code_arr[ip + 2] == 0)            \
        {                                                                                   \
            fprintf (file_ptr, "%s %cx\n",                                                  \
                               Get_CMD_Name (num),                                          \
                               code_arr[ip + 1] + 'a' - 1);                                 \
            ip += 3;                                                                        \
        }                                                                                   \
        if (code_arr[ip] == 0 && code_arr[ip + 1] == 0 && code_arr[ip + 2] == 1)            \
        {                                                                                   \
            fprintf (file_ptr, "push %f\n",                                                 \
                               *(double *)(code_arr + ip + 3));                             \
            ip += 3 + sizeof (double);                                                      \
        }                                                                                   \
        if (code_arr[ip] == 0 && code_arr[ip + 1] == 0 && code_arr[ip + 2] == 0)            \
        {                                                                                   \
            fprintf (file_ptr, "pop\n");                                                    \
            ip += 3;                                                                        \
        }                                                                                   \
    }                                                                                       \
    else if (Check_If_Jump (num) == 1)                                                      \
    {                                                                                       \
        fprintf (file_ptr, "%s %d\n", Get_CMD_Name (num), *(int *)(code_arr + ip + 1));     \
        ip += 1 + sizeof (int);                                                             \
    }                                                                                       \
    else                                                                                    \
    {                                                                                       \
        fprintf (file_ptr, "%s %lf\n",                                                      \
                           Get_CMD_Name (num),                                              \
                           *(double *)(code_arr + ip + 1));                                 \
        ip += 1 + sizeof (double);                                                          \
    }                                                                                       \
                                                                                            \
    break;                                                              

static int Second_Passing (const char *code_arr, const int max_ip, FILE *file_ptr, const char *label_arr, const int n_labels)
{
    MY_ASSERT (code_arr,  "const char *code_arr",  NULL_PTR, ERROR);
    MY_ASSERT (label_arr, "const char *label_arr", NULL_PTR, ERROR);

    for (int ip = 0; ip < max_ip; )
    {
        for (int i = 0; i <= n_labels; i++)
        {
            if (label_arr[i] == ip && label_arr[i] != 0)
                fprintf (file_ptr, "%d:\n", label_arr[i]);
        }

        switch (code_arr[ip])
        {
            #include "../Commands_List.h"

            default: MY_ASSERT (false, "code_arr[ip]", UNEXP_SYMB, ERROR);
        }
    }

    return NO_ERRORS;
}
#undef DEFCMD_

int Disassemble (const char *input_name, const char *output_name)
{
    MY_ASSERT (input_name, "const char *code", NULL_PTR, ERROR);

    long max_ip = 0;
    char *code = Make_File_Buffer (input_name, &max_ip);
    MY_ASSERT (code != NULL, "Make_File_Buffer ()", FUNC_ERROR, ERROR);

    FILE *file_ptr = Open_File (output_name, "wb");

    int n_labels = 0;
    char *label_arr = First_Passing (code, max_ip, &n_labels);
    MY_ASSERT (label_arr, "First_Passing ()", FUNC_ERROR, ERROR);

    for (int i = 0; i < n_labels - 1; i++)
        for (int j = i + 1; j < n_labels; j++)
            if (label_arr[i] == label_arr[j])
                label_arr[i] = 0;

    if (Second_Passing (code, max_ip, file_ptr, label_arr, n_labels) == ERROR)
        MY_ASSERT (false, "Second_Passing ()", FUNC_ERROR, ERROR);

    free (label_arr);
    free (code);

    Close_File (file_ptr, output_name);

    return NO_ERRORS;
}
