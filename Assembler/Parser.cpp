#include "Assembler.h"
#include "Reading_File.h"
#include "../Log_File.h"

extern const char *BINARY_FILE;

int Make_Binary (const struct Token *token_arr, const int n_tokens)
{
    MY_ASSERT (token_arr, "const struct Token *token_arr", NULL_PTR, ERROR);

    FILE *file_ptr = Open_File (BINARY_FILE, "wb");

    struct Label *label_arr = (struct Label *)calloc (n_tokens, sizeof (struct Label));
    MY_ASSERT (label_arr, "struct Label *label_arr", NE_MEM, ERROR);
    int n_labels = 0;

    size_t code_size = 0;
    
    char *code = First_Passing (token_arr, n_tokens, label_arr, &n_labels, &code_size);
    MY_ASSERT (code != NULL, "First_Passing ()", FUNC_ERROR, ERROR);

    if (Second_Passing (token_arr, n_tokens, label_arr, n_labels, code) == ERROR)
        MY_ASSERT (false, "Second_Passing ()", FUNC_ERROR, ERROR);

    fwrite (code, sizeof (char), code_size, file_ptr);
    
    free (label_arr);
    free (code);

    Close_File (file_ptr, BINARY_FILE);

    return NO_ERRORS;
}

//**************************************************************
#define DEFCMD_(num, name, n_args, code)    \
do                                          \
{                                           \
    if (cmd_num == num)                     \
    {                                       \
        if (strcmp ("pop", #name) == 0)     \
            return 1;                       \
        else                                \
            return 0;                       \
    }                                       \
}                                           \
while (0)

int Check_If_Pop (const int cmd_num)
{
    #include "../Commands_List.h"

    MY_ASSERT (false, "const int cmd_num", UNDEF_CMD, ERROR);
}
#undef DEFCMD_
//**************************************************************
char *First_Passing (const struct Token *token_arr, const int n_tokens, struct Label *label_arr, int *n_labels, size_t *code_size)
{
    MY_ASSERT (token_arr, "const struct Token *token_arr", NULL_PTR, NULL);
    MY_ASSERT (label_arr, "struct Label *label_arr",       NULL_PTR, NULL);
    MY_ASSERT (n_labels,  "int *n_labels",                 NULL_PTR, NULL);
    MY_ASSERT (code_size, "size_t *code_size",             NULL_PTR, NULL);
    
    char *code = (char *)calloc (n_tokens * sizeof (double), sizeof (char));
    MY_ASSERT (code, "char *code", NE_MEM, NULL);
    size_t ip = 0;

    int label_i = 0;

    for (int i = 0; i < n_tokens; i++)
    {
        switch (token_arr[i].type)
        {
            case CMD:
                code[ip++] = token_arr[i].value.cmd_num;
                if (i > 0 && token_arr[i - 1].type == CMD && Check_If_Pop (token_arr[i - 1].value.cmd_num) == 1)
                    ip += 3;
                break;
            case BRACKET:
                if (token_arr[i].value.bracket == '[')
                    code[ip++] = 1;

                break;
            case REG:
                if (token_arr[i - 1].type == CMD)
                    ip++;
                code[ip++] = token_arr[i].value.reg_name;

                if (i < n_tokens - 1 && token_arr[i + 1].type == BRACKET && token_arr[i + 1].value.bracket == ']')
                    ip++;

                if (token_arr[i - 1].type != BRACKET)
                    ip++;

                break;
            case PLUS:
                break;
            case NUM:
                switch (token_arr[i - 1].type)
                {
                    case BRACKET:
                        ip++;
                    case PLUS:
                        code[ip++] = 1;
                        *(int *)(code + ip) = (int)token_arr[i].value.number;
                        ip += sizeof (int);
                        break;
                    case CMD:
                        if (Check_If_Push_Pop (token_arr[i - 1].value.cmd_num) == 1)
                        {
                            if (Check_If_Pop (token_arr[i].value.cmd_num) == 1)
                                MY_ASSERT (false, "\"pop\" instruction", INCORR_ARG, NULL);
                            ip += 2;
                            code[ip++] = 1;
                        }
                        *(double *)(code + ip) = token_arr[i].value.number;
                        ip += sizeof (double);
                        break;
                    case REG: case NUM: case LBL: case JMP_ARG:
                        break;
                }
                break;
            case LBL:
                memmove (label_arr[label_i].name, token_arr[i].value.label, MAX_NAME_SIZE);
                label_arr[label_i].next_cmd_ip = ip;
                label_i++;
                break;
            case JMP_ARG:
                ip++;
                break;

            default: MY_ASSERT (false, "token_arr[i].type", UNEXP_VAL, NULL);
        }
    }

    *code_size = ip;
    *n_labels = label_i;

    return code;
}

int Second_Passing (const struct Token *token_arr, const int n_tokens, const struct Label *label_arr, const int n_labels, char *code)
{
    MY_ASSERT (code,      "char *code",                    NULL_PTR, ERROR);
    MY_ASSERT (label_arr, "struct Label *label_arr",       NULL_PTR, ERROR);
    MY_ASSERT (token_arr, "const struct Token *token_arr", NULL_PTR, ERROR);

    int ip = 0, label_i = 0;

    for (int i = 0; i < n_tokens; i++)
    {
        switch (token_arr[i].type)
        {
            case JMP_ARG:
                label_i = Find_Label (label_arr, n_labels, token_arr[i].value.jmp_arg);
                MY_ASSERT (label_i != ERROR, "Find_Label ()", FUNC_ERROR, ERROR);
                
                if (label_i == -1)
                {
                    printf ("UNKNOWN JUMP: %s\n", token_arr[i].value.jmp_arg);
                    MY_ASSERT (false, "None", UNKNOWN_JMP, ERROR);
                }

                code[ip++] = label_arr[label_i].next_cmd_ip;
                break;
            case CMD:
                ip++;
                if (i > 0 && token_arr[i - 1].type == CMD && Check_If_Pop (token_arr[i - 1].value.cmd_num) == 1)
                    ip += 3;
                break;
            case BRACKET:
                if (token_arr[i].value.bracket == '[')
                    ip++;
                break;
            case REG:
                if (token_arr[i - 1].type == CMD)
                    ip++;
                ip++;

                if (i < n_tokens - 1 && token_arr[i + 1].type == BRACKET && token_arr[i + 1].value.bracket == ']')
                    ip++;

                if (i < n_tokens - 1 && token_arr[i + 1].type == CMD)
                    ip++;

                break;
            case PLUS:
            case LBL:
                break;
            case NUM:
                switch (token_arr[i - 1].type)
                {
                    case BRACKET:
                        ip++;
                    case PLUS:
                        ip += sizeof (int) + 1;
                        break;
                    case CMD:
                        if (Check_If_Push_Pop (token_arr[i - 1].value.cmd_num) == 1)
                            ip += 3;
                        ip += sizeof (double);
                        break;
                    default: break;
                }
                break;
        }
    }

    return NO_ERRORS;
}

int Find_Label (const struct Label *label_arr, const int n_labels, const char *label_name)
{
    MY_ASSERT (label_arr,  "const struct Label *label_arr", NULL_PTR, ERROR);
    MY_ASSERT (label_name, "const char *label_name",        NULL_PTR, ERROR);
    
    for (int i = 0; i < n_labels; i++)
    {
        if (strcmp (label_arr[i].name, label_name) == 0)
            return i;
    }

    return -1;
}