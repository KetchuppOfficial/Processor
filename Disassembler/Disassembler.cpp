#include "Disassembler.h"
#include "Reading_File.h"
#include "../Log_File.h"

extern const char *BINARY_FILE;
extern const char *DISASM_FILE;

char *Get_Code (int *max_ip)
{
    FILE *file_ptr = Open_File (BINARY_FILE, "rb");

    int n_symbs = (int)Define_File_Size (file_ptr);

    char *code = (char *)calloc (n_symbs, sizeof (char));
    MY_ASSERT (code, "char *code", NE_MEM, NULL);

    fread (code, sizeof (char), n_symbs, file_ptr);

    Close_File (file_ptr, BINARY_FILE);

    *max_ip = n_symbs;

    return code;
}

int Disassemble (const char *code, const int max_ip)
{
    MY_ASSERT (code, "const char *code", NULL_PTR, ERROR);

    FILE *file_ptr = Open_File (DISASM_FILE, "wb");

    int n_labels = 0;
    char *label_arr = First_Passing (code, max_ip, &n_labels);
    MY_ASSERT (label_arr, "First_Passing ()", FUNC_ERROR, ERROR);

    for (int i = 0; i < n_labels; i++)
        for (int j = i + 1; j < n_labels; j++)
            if (label_arr[i] == label_arr[j])
                label_arr[i] = 0;

    if (Second_Passing (code, max_ip, file_ptr, label_arr, n_labels) == ERROR)
        MY_ASSERT (false, "Second_Passing ()", FUNC_ERROR, ERROR);

    free (label_arr);

    Close_File (file_ptr, DISASM_FILE);

    return NO_ERRORS;
}

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
        label_arr[label_i++] = code_arr[ip++];                                      \
    }                                                                               \
    else                                                                            \
        ip += (1 + sizeof (double));                                                \
                                                                                    \
    break;                                                              

char *First_Passing (const char *code_arr, const int max_ip, int *n_labels)
{
    MY_ASSERT (code_arr, "const char *code", NULL_PTR, NULL);

    char *label_arr = (char *)calloc (max_ip, sizeof (char));
    MY_ASSERT (label_arr, "char *label_arr", NE_MEM, NULL);
    int label_i = 0;

    for (int ip = 0; ip < max_ip; )
    {
        switch (code_arr[ip])
        {
            #include "../Commands_List.h"

            default: 
            MY_ASSERT (false, "code_arr[ip]", UNDEF_CMD, NULL);
        }
    }

    *n_labels = label_i;

    return label_arr;
}
#undef DEFCMD_

//***************************************************************
#define DEFCMD_(num, name, n_args, code)    \
do                                          \
{                                           \
    if (cmd_n == num)                       \
        return n_args;                      \
}                                           \
while (0)

int Check_N_Args (const int cmd_n)
{           
    #include "../Commands_List.h"

    return 0;
}
#undef DEFCMD_
//***************************************************************

//***************************************************************
#define DEFCMD_(num, name, n_args, code)        \
do                                              \
{                                               \
    if (cmd_num == num)                         \
    {                                           \
        if (strcmp ("jae",  #name)  == 0 ||     \
            strcmp ("jbe",  #name)  == 0 ||     \
            strcmp ("ja",   #name)  == 0 ||     \
            strcmp ("jb",   #name)  == 0 ||     \
            strcmp ("ja",   #name)  == 0 ||     \
            strcmp ("je",   #name)  == 0 ||     \
            strcmp ("call", #name)  == 0)       \
            return 1;                           \
        else                                    \
            return 0;                           \
    }                                           \
}                                               \
while (0)

int Check_If_Jump (const int cmd_num)
{
    #include "../Commands_List.h"

    MY_ASSERT (false, "const int cmd_num", UNDEF_CMD, ERROR);
}
#undef DEFCMD_
//***************************************************************

//***************************************************************
#define DEFCMD_(num, name, n_args, code)        \
do                                              \
{                                               \
    if (num == pp_num)                          \
    {                                           \
        if (strcmp (#name, "push") == 0 ||      \
            strcmp (#name, "pop")  == 0)        \
            return 1;                           \
        else                                    \
            return 0;                           \
    }                                           \
}                                               \
while (0)

int Check_If_Push_Pop (const int pp_num)
{
    #include "../Commands_List.h"

    MY_ASSERT (false, "const int cmd_num", UNDEF_CMD, ERROR);
}
#undef DEFCMD_
//***************************************************************

#define DEFCMD_(num, name, n_args, code)                                            \
case num:                                                                           \
    if (Check_N_Args (num) == 0)                                                    \
    {                                                                               \
        fprintf (file_ptr, "%s\n", Show_CMD_Name (num));                            \
        ip++;                                                                       \
    }                                                                               \
    else if (Check_If_Push_Pop (num) == 1)                                          \
    {                                                                               \
        ip++;                                                                       \
        if (code_arr[ip] == 1 && code_arr[ip + 1] != 0 && code_arr[ip + 2] == 1)    \
        {                                                                           \
            fprintf (file_ptr, "%s [%cx + %d]\n",                                   \
                               Show_CMD_Name (num),                                 \
                               code_arr[ip + 1] + 'a',                              \
                               *(int *)(code_arr + ip + 3));                        \
            ip += 3 + sizeof (int);                                                 \
        }                                                                           \
        if (code_arr[ip] == 1 && code_arr[ip + 1] != 0 && code_arr[ip + 2] == 0)    \
        {                                                                           \
            fprintf (file_ptr, "%s [%cx]\n",                                        \
                               Show_CMD_Name (num),                                 \
                               code_arr[ip + 1] + 'a');                             \
            ip += 3;                                                                \
        }                                                                           \
        if (code_arr[ip] == 1 && code_arr[ip + 1] == 0 && code_arr[ip + 2] == 1)    \
        {                                                                           \
            fprintf (file_ptr, "%s [%d]\n",                                         \
                               Show_CMD_Name (num),                                 \
                               *(int *)(code_arr + ip + 3));                        \
            ip += 3 + sizeof (int);                                                 \
        }                                                                           \
        if (code_arr[ip] == 0 && code_arr[ip + 1] != 0 && code_arr[ip + 2] == 0)    \
        {                                                                           \
            fprintf (file_ptr, "%s %cx\n",                                          \
                               Show_CMD_Name (num),                                 \
                               code_arr[ip + 1] + 'a' - 1);                         \
            ip += 3;                                                                \
        }                                                                           \
        if (code_arr[ip] == 0 && code_arr[ip + 1] == 0 && code_arr[ip + 2] == 1)    \
        {                                                                           \
            fprintf (file_ptr, "push %d\n",                                         \
                               *(int *)(code_arr + ip + 3));                        \
            ip += 3 + sizeof (double);                                              \
        }                                                                           \
        if (code_arr[ip] == 0 && code_arr[ip + 1] == 0 && code_arr[ip + 2] == 0)    \
        {                                                                           \
            fprintf (file_ptr, "pop\n");                                            \
            ip += 3;                                                                \
        }                                                                           \
    }                                                                               \
    else if (Check_If_Jump (num) == 1)                                              \
    {                                                                               \
        fprintf (file_ptr, "%s %d\n", Show_CMD_Name (num), code_arr[ip + 1]);       \
        ip += 2;                                                                    \
    }                                                                               \
    else                                                                            \
    {                                                                               \
        fprintf (file_ptr, "%s %lf\n",                                              \
                           Show_CMD_Name (num),                                     \
                           *(double *)(code_arr + ip + 1));                         \
        ip += 1 + sizeof (double);                                                  \
    }                                                                               \
                                                                                    \
    break;                                                              

int Second_Passing (const char *code_arr, const int max_ip, FILE *file_ptr, const char *label_arr, const int n_labels)
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

            default: MY_ASSERT (false, "code_arr[ip]", UNDEF_CMD, ERROR);
        }
    }

    return NO_ERRORS;
}
#undef DEFCMD_

#define DEFCMD_(num, name, n_args, code)    \
do                                          \
{                                           \
    if (cmd_n == num)                       \
        return #name;                       \
}                                           \
while (0)

const char *Show_CMD_Name (const int cmd_n)
{
    #include "../Commands_List.h"

    return NULL;
}
#undef DEFCMD_