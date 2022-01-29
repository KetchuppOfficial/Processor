#include "Processor.h"
#include "Reading_File.h"
#include "../Log_File.h"

extern const char *BINARY;

char *Get_Code (int *max_ip)
{
    MY_ASSERT (max_ip, "int *max_ip", NULL_PTR, NULL);
    
    FILE *file_ptr = Open_File (BINARY_FILE, "rb");

    int n_symbs = (int)Define_File_Size (file_ptr);

    char *code = (char *)calloc (n_symbs, sizeof (char));
    MY_ASSERT (code, "char *code", NE_MEM, NULL);

    fread (code, sizeof (char), n_symbs, file_ptr);

    Close_File (file_ptr, BINARY_FILE);

    *max_ip = n_symbs;

    return code;
}

#define DEFCMD_(num, name, n_args, code)                                            \
case num:                                                                           \
    if (Check_N_Args (num) == 0)                                                    \
        ip++;                                                                       \
    else if (Check_If_Push_Pop (num) == 1)                                          \
    {                                                                               \
        code;                                                                       \
        ip++;                                                                       \
        if (code_arr[ip] == 1 && code_arr[ip + 1] != 0 && code_arr[ip + 2] == 1)    \
        {                                                                           \
            ip += (3 + sizeof (int));                                               \
        }                                                                           \
        if (code_arr[ip] == 1 && code_arr[ip + 1] != 0 && code_arr[ip + 2] == 0)    \
        {                                                                           \
            ip += 3;                                                                \
        }                                                                           \
        if (code_arr[ip] == 1 && code_arr[ip + 1] == 0 && code_arr[ip + 2] == 1)    \
        {                                                                           \
            ip += (3 + sizeof (int));                                               \
        }                                                                           \
        if (code_arr[ip] == 0 && code_arr[ip + 1] != 0 && code_arr[ip + 2] == 0)    \
        {                                                                           \
            ip += 3;                                                                \
        }                                                                           \
        if (code_arr[ip] == 0 && code_arr[ip + 1] == 0 && code_arr[ip + 2] == 1)    \
        {                                                                           \
            ip += (3 + sizeof (double));                                            \
        }                                                                           \
        if (code_arr[ip] == 0 && code_arr[ip + 1] == 0 && code_arr[ip + 2] == 0)    \
        {                                                                           \
            ip += 3;                                                                \
        }                                                                           \
    }                                                                               \
    else if (Check_If_Jump (num) == 1)                                              \
    {                                                                               \
        code;                                                                       \
        ip++;                                                                       \
        label_arr[label_i++] = code_arr[ip++];                                      \
    }                                                                               \
    else                                                                            \
    {                                                                               \
        code;                                                                       \
        ip += (1 + sizeof (double));                                                \
    }                                                                               \
                                                                                    \
    break;                                                              

int Process_Code (const char *code_arr, const int max_ip)
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
#define DEFCMD_(num, name, n_args, code)        \
do                                              \
{                                               \
    if (num == cmd_num)                         \
    {                                           \
        if (strcmp (#name, "push") == 0 ||      \
            strcmp (#name, "pop")  == 0)        \
            return 1;                           \
        else                                    \
            return 0;                           \
    }                                           \
}                                               \
while (0)

int Check_If_Push_Pop (const int cmd_num)
{
    #include "../Commands_List.h"

    MY_ASSERT (false, "const int cmd_num", UNDEF_CMD, ERROR);
}
#undef DEFCMD_
//***************************************************************

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