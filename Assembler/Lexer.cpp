#include "Assembler.h"
#include "../Reading_File.h"
#include "../Log_File.h"

#define DEBUG 0

extern const char *CMD_FILE_NAME;
extern FILE *LOG_FILE;

struct Token *Read_Asm (int *n_tokens)
{
    MY_ASSERT (n_tokens, "int *n_tokens", NULL_PTR, NULL);
    
    FILE *cmd_file = Open_File (CMD_FILE_NAME, "rb");

    long n_symbs = Define_File_Size (cmd_file);

    char *buffer = Make_Buffer (cmd_file, n_symbs);

    struct Token *token_arr = Lexer (buffer, n_symbs, n_tokens);
    if (token_arr == NULL)
        MY_ASSERT (false, "Lexer ()", FUNC_ERROR, NULL);

    #if DEBUG == 1
    printf ("\n\n");
    for (int i = 0; i < *n_tokens; i++)
    {
        switch (token_arr[i].type)
        {
            case CMD:      printf ("command: %s\n", Show_CMD_Name (token_arr[i].value.cmd_num));    break;
            case BRACKET:  printf ("bracket: %c\n",        token_arr[i].value.bracket);             break;
            case REG:      printf ("register name: %cx\n", token_arr[i].value.reg_name + 'a' - 1);  break;
            case PLUS:     printf ("plus: %c\n",           token_arr[i].value.plus);                break;
            case NUM:      printf ("number: %f\n",         token_arr[i].value.number);              break;
            case LBL:      printf ("label: %s\n",          token_arr[i].value.label);               break;
            case JMP_ARG:  printf ("jump arg: %s\n",       token_arr[i].value.jmp_arg);             break;

            default: printf ("Some shit happened\n");
        }
    }
    #endif

    free (buffer);

    Close_File (cmd_file, CMD_FILE_NAME);

    return token_arr;
}

//*************************************************************************
#if DEBUG == 1
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
#endif
//*************************************************************************

struct Token *Lexer (const char *str, const long n_symbs, int *n_tokens)
{
    MY_ASSERT (str,         "const char *str",    NULL_PTR, NULL);
    MY_ASSERT (n_symbs > 0, "const long n_symbs", POS_VAL,  NULL);
    MY_ASSERT (n_tokens,    "int *n_toknes",      NULL_PTR, NULL);

    struct Token *token_arr = (struct Token *)calloc (n_symbs, sizeof (struct Token));
    MY_ASSERT (token_arr, "struct Token *token_arr", NE_MEM, NULL);

    struct Argument arg = {str, n_symbs, 0, token_arr, 0, 1};

    while (arg.symb_i < arg.n_symbs)
    {
        if (Process_Line (&arg) == ERROR)
            MY_ASSERT (false, "Process_Line ()", FUNC_ERROR, NULL);

        arg.line++;
    }
        
    *n_tokens = arg.token_i;

    return token_arr;
}

int Process_Line (struct Argument *arg)
{
    MY_ASSERT (arg, "struct Argument *arg", NULL_PTR, ERROR);
    
    if (Skip_Spaces (arg, MOL) == ERROR)
        MY_ASSERT (false, "Skip_Spaces ()", FUNC_ERROR, ERROR);

    if (Skip_Comments (arg) == ERROR)
        MY_ASSERT (false, "Skip_Comments ()", FUNC_ERROR, ERROR);

    int n_args = Process_Name (arg);
    MY_ASSERT (n_args != ERROR, "Process_Name ()", FUNC_ERROR, ERROR);

    if (n_args > 0)
    {
        if (Skip_Spaces (arg, MOL) == ERROR)
            MY_ASSERT (false, "Skip_Spaces ()", FUNC_ERROR, ERROR);

        if (Process_Arg (arg) == ERROR)
            MY_ASSERT (false, "Process_Arg ()", FUNC_ERROR, ERROR);
    }

    while (arg->symb_i < arg->n_symbs && isspace (arg->str[arg->symb_i]) && 
           arg->str[arg->symb_i] != '\n' && arg->str[arg->symb_i] != '\r')
        arg->symb_i++;

    if (arg->symb_i < arg->n_symbs && arg->str[arg->symb_i] == ';')
        Skip_Comments (arg);
    else if (!isspace (arg->str[arg->symb_i]))
    {
        Show_Error (arg, "Unexpected symbol");
        MY_ASSERT (false, "Buffer with assembler code", UNEXP_SYMB, ERROR);
    }

    while (arg->symb_i < arg->n_symbs && (arg->str[arg->symb_i] == '\n' || arg->str[arg->symb_i] == '\r'))
        arg->symb_i++;
    
    return NO_ERRORS;
}

#define Add_Token(arg, type, token)                             \
do                                                              \
{                                                               \
    if (Add_Token_ (arg, type, 1, token) == ERROR)              \
        MY_ASSERT (false, "Add_Token_ ()", FUNC_ERROR, ERROR);  \
}                                                               \
while (0)

int Process_Name (struct Argument *arg)
{
    MY_ASSERT (arg, "struct Argument *arg", NULL_PTR, ERROR);

    char name_arr[MAX_NAME_SIZE] = "";

    int cmd_num = 0, n_args = 0;

    int i = 0;
    for (i = 0; isalpha (arg->str[arg->symb_i]) && arg->str[arg->symb_i] != ':'; i++, arg->symb_i++)
        name_arr[i] = arg->str[arg->symb_i];
    
    if (arg->str[arg->symb_i] == ':')
    {
        Add_Token (arg, LBL, name_arr);
        arg->symb_i++;
    }
    else
    {
        cmd_num = Check_Name (arg, name_arr);
        MY_ASSERT (cmd_num != ERROR, "Check_Name ()", FUNC_ERROR, ERROR);
            
        Add_Token (arg, CMD, cmd_num);

        n_args = Check_N_Args (arg, cmd_num);
        MY_ASSERT (n_args != ERROR, "Check_N_Args ()", FUNC_ERROR, ERROR);
    }

    return n_args;
}

//*************************************************************************
#define DEFCMD_(num, name, n_args, code)    \
do                                          \
{                                           \
    if (strcmp (name_arr, #name) == 0)      \
        return num;                         \
}                                           \
while (0)

int Check_Name (struct Argument *arg, const char *name_arr)
{
    MY_ASSERT (name_arr, "const char *name_arr", NULL_PTR, ERROR);
    MY_ASSERT (arg,      "struct Argument *arg", NULL_PTR, ERROR);
    
    for (int i = 0; ; i++)
    {
        if (i > 0)
        {
            Show_Error (arg, "There is no command with this name");
            MY_ASSERT (false, "const char *name_arr", UNDEF_CMD, ERROR);
        }

        #include "../Commands_List.h"
    }
}

#undef DEFCMD_
//*************************************************************************

//*************************************************************************
#define DEFCMD_(num, name, n_args, code)    \
do                                          \
{                                           \
    if (cmd_n == num)                       \
        return n_args;                      \
}                                           \
while (0)

int Check_N_Args (struct Argument *arg, const int cmd_n)
{    
    MY_ASSERT (arg, "struct Argument *arg", NULL_PTR, ERROR);
    
    for (int i = 0; ; i++)
    {
        if (i > 0)
        {
            Show_Error (arg, "There is no command with this number");
            MY_ASSERT (false, "const int *cmd_n", UNDEF_CMD, ERROR);
        }
        
        #include "../Commands_List.h"
    }
}
#undef DEFCMD_
//*************************************************************************

int Process_Arg (struct Argument *arg)
{
    MY_ASSERT (arg, "struct Argument *arg", NULL_PTR, ERROR);
    
    if (isalpha (arg->str[arg->symb_i]) && Check_If_Jump (arg->token_arr[arg->token_i - 1].value.cmd_num))
    {
        char arg_arr[MAX_NAME_SIZE] = "";

        int i = 0;
        for (i = 0; isalpha (arg->str[arg->symb_i]); i++, arg->symb_i++)
            arg_arr[i] = arg->str[arg->symb_i];

        Add_Token (arg, JMP_ARG, arg_arr);    
    }
    else if (Check_If_Push_Pop (arg->token_arr[arg->token_i - 1].value.cmd_num))
    {
        if (Get_Brackets (arg) == ERROR)
            MY_ASSERT (false, "Get_Brackets ()", FUNC_ERROR, ERROR);
    }
    else
    {
        if (Get_Number (arg) == ERROR)
            MY_ASSERT (false, "Get_Number ()", FUNC_ERROR, ERROR);
    }

    return NO_ERRORS;
}

//*************************************************************************
#define DEFCMD_(num, name, n_args, code)        \
do                                              \
{                                               \
    if (cmd_num == num)                         \
    {                                           \
        if (strcmp ("jae",  #name) == 0 ||      \
            strcmp ("jbe",  #name) == 0 ||      \
            strcmp ("ja",   #name) == 0 ||      \
            strcmp ("jb",   #name) == 0 ||      \
            strcmp ("ja",   #name) == 0 ||      \
            strcmp ("je",   #name) == 0 ||      \
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
//*************************************************************************

//*************************************************************************
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

    MY_ASSERT (false, "const int pp_num", UNDEF_CMD, ERROR);
}
#undef DEFCMD_
//*************************************************************************

int Get_Brackets (struct Argument *arg)
{
    MY_ASSERT (arg, "struct Argument *arg", NULL_PTR, ERROR);

    if (arg->str[arg->symb_i] == '[')
    {
        Add_Token (arg, BRACKET, '[');

        arg->symb_i++;
        if (Skip_Spaces (arg, MOL) == ERROR)
            MY_ASSERT (false, "Skip_Spaces ()", FUNC_ERROR, ERROR);
        
        if (Get_Inside (arg) == ERROR)
            MY_ASSERT (false, "Get_Inside ()", FUNC_ERROR, ERROR);

        Require (arg, ']');

        Add_Token (arg, BRACKET, ']');

        if (Skip_Spaces (arg, EOL) == ERROR)
            MY_ASSERT (false, "Skip_Spaces ()", FUNC_ERROR, ERROR);

        return NO_ERRORS;
    }
    else if (isdigit (arg->str[arg->symb_i]))
    {        
        if (Get_Number (arg) == ERROR)
            MY_ASSERT (false, "Get_Number ()", FUNC_ERROR, ERROR);
    }
    else if (isalpha (arg->str[arg->symb_i]))
    {      
        if (Get_Reg (arg) == ERROR)
            MY_ASSERT (false, "Get_Reg ()", FUNC_ERROR, ERROR);
    }

    return NO_ERRORS;
}

int Require (struct Argument *arg, const char symb)
{
    MY_ASSERT (arg, "struct Argument *arg", NULL_PTR, ERROR);
    
    if (arg->str[arg->symb_i] == symb)
        arg->symb_i++;
    else
    {
        Show_Error (arg, "Requiered symbol hasn't been found");
        MY_ASSERT (false, "Buffer with assembler code", UNEXP_SYMB, ERROR);
    }
    
    return NO_ERRORS;
}

int Get_Inside (struct Argument *arg)
{
    MY_ASSERT (arg, "struct Argument *arg", NULL_PTR, ERROR);
    
    if (isdigit (arg->str[arg->symb_i]))
    {
        if (Get_Number (arg) == ERROR)
            MY_ASSERT (false, "Get_Number ()", FUNC_ERROR, ERROR);

        if (Skip_Spaces (arg, MOL) == ERROR)
            MY_ASSERT (false, "Skip_Spaces ()", FUNC_ERROR, ERROR);
    }
    else if (isalpha (arg->str[arg->symb_i]))
    {
        if (Get_Reg (arg) == ERROR)
            MY_ASSERT (false, "Get_Reg ()", FUNC_ERROR, ERROR);

        if (Skip_Spaces (arg, MOL) == ERROR)
            MY_ASSERT (false, "Skip_Spaces ()", FUNC_ERROR, ERROR);
            
        if (arg->str[arg->symb_i] == '+')
        {
            Add_Token (arg, PLUS, '+');
            arg->symb_i++;

            if (Skip_Spaces (arg, MOL) == ERROR)
                MY_ASSERT (false, "Skip_Spaces ()", FUNC_ERROR, ERROR);
            
            if (isdigit (arg->str[arg->symb_i]))
            {
                if (Get_Number (arg) == ERROR)
                    MY_ASSERT (false, "Get_Number ()", FUNC_ERROR, ERROR);

                if (Skip_Spaces (arg, MOL) == ERROR)
                    MY_ASSERT (false, "Skip_Spaces ()", FUNC_ERROR, ERROR);
            }
            else
            {
                Show_Error (arg, "Plus sign has to be followed by a number");
                MY_ASSERT (false, "Buffer with assemler code", UNEXP_SYMB, ERROR);
            }
        }
        else if (arg->str[arg->symb_i] == ']')
            return NO_ERRORS;
        else
        {
            Show_Error (arg, "Only plus sign can follow the name of a register");
            MY_ASSERT (false, "Buffer with assembler code", UNEXP_SYMB, ERROR);
        }
    }
    else
    {
        Show_Error (arg, "Only the name of a register or a number can follow left bracket");
        MY_ASSERT (false, "Buffer with assembler code", UNEXP_SYMB, ERROR);
    }

    return NO_ERRORS;
}

int Get_Number (struct Argument *arg)
{
    MY_ASSERT (arg, "struct Argument *arg", NULL_PTR, ERROR);

    double num = 0.0;

    while ('0' <= arg->str[arg->symb_i] && arg->str[arg->symb_i] <= '9')
    {
        num = num * 10 + (arg->str[arg->symb_i] - '0');
        arg->symb_i++;
    }

    if (arg->str[arg->symb_i] == '.')
    {
        arg->symb_i++;
        const char *old_str = arg->str + arg->symb_i;

        int degree = -1;

        while ('0' <= arg->str[arg->symb_i] && arg->str[arg->symb_i] <= '9')
        {
            num += (arg->str[arg->symb_i] - '0') * pow (10, degree);
            degree--;
            arg->symb_i++;
        }

        if (old_str == arg->str + arg->symb_i)
        {
            Show_Error (arg, "No digits after decimal point have been found");
            MY_ASSERT (false, "const char *old_str", WRONG_DECIMAL, ERROR);
        }
    }

    Add_Token (arg, NUM, num);

    return NO_ERRORS;
}

int Get_Reg (struct Argument *arg)
{
    MY_ASSERT (arg, "struct Argument *arg", NULL_PTR, ERROR);
    
    if ('a' <= arg->str[arg->symb_i] && arg->str[arg->symb_i] <= 'd')
    {
        if (arg->str[arg->symb_i + 1] == 'x')
            Add_Token (arg, REG, arg->str[arg->symb_i] - 'a' + 1);
        else
        {
            Show_Error (arg, "The full name of a register should end by letter \"x\"");
            MY_ASSERT (false, "Buffer with assembler code", UNEXP_SYMB, ERROR);
        }        
    }
    else
    {
        Show_Error (arg, "The name of a register has to start with of of 4 letters: \"a\", \"b\", \"c\" or \"d\"");
        MY_ASSERT (false, "Buffer with assembler code", UNEXP_SYMB, ERROR);
    }    

    arg->symb_i += 2;

    return NO_ERRORS;
}

int Add_Token_ (struct Argument *arg, Types type, int parmN, ...)
{
    MY_ASSERT (arg, "struct Argument *arg", NULL_PTR, ERROR);
    
    va_list ap;
    va_start (ap, parmN);

    arg->token_arr[arg->token_i].type = type;

    switch (type)
    {
        case CMD:
            arg->token_arr[arg->token_i].value.cmd_num  = va_arg (ap, int);
            break;
        case BRACKET:
            arg->token_arr[arg->token_i].value.bracket  = va_arg (ap, int);
            break;
        case REG:
            arg->token_arr[arg->token_i].value.reg_name = va_arg (ap, int);
            break;
        case PLUS:
            arg->token_arr[arg->token_i].value.plus     = va_arg (ap, int);
            break;
        case NUM:
            arg->token_arr[arg->token_i].value.number   = va_arg (ap, double);
            break;
        case LBL:
            memmove (arg->token_arr[arg->token_i].value.label, va_arg (ap, char *), MAX_NAME_SIZE);
            break;
        case JMP_ARG:
            memmove (arg->token_arr[arg->token_i].value.jmp_arg, va_arg (ap, char *), MAX_NAME_SIZE);
            break;
        default:
            va_end (ap);
            MY_ASSERT (false, "Types type", UNEXP_VAL, ERROR);
    }
    arg->token_i++;

    va_end (ap);
    
    return NO_ERRORS;
}

int Show_Error (struct Argument *arg, const char *err_description)
{
    MY_ASSERT (arg,             "struct Argument *arg",        NULL_PTR, ERROR);
    MY_ASSERT (err_description, "const char *err_description", NULL_PTR, ERROR);

    long line_beginning = arg->symb_i;

    while (line_beginning >= 0 && arg->str[line_beginning] != '\n')
        line_beginning--;
    line_beginning++;

    long shift = arg->symb_i - line_beginning;
    
    fprintf (LOG_FILE, "\n***************************************************************************************************\n");
    fprintf (LOG_FILE, "LINE %d:\n", arg->line);
    for (long i = 0; line_beginning + i < arg->n_symbs && arg->str[line_beginning + i] != '\r' && arg->str[line_beginning + i] != '\n'; i++)
        fprintf (LOG_FILE, "%c", arg->str[line_beginning + i]);
    fprintf (LOG_FILE, "\n");
    
    for (long i = 0; i < shift; i++)
    {
        if (arg->str[line_beginning + i] == '\t')
            fprintf (LOG_FILE, "\t");
        else
            fprintf (LOG_FILE, " ");
    }
    fprintf (LOG_FILE, "^~~~~~~~~~~ %s", err_description);

    fprintf (LOG_FILE, "\n***************************************************************************************************\n");

    return NO_ERRORS;
}

int Skip_Comments (struct Argument *arg)
{
    MY_ASSERT (arg, "struct Argument *arg", NULL_PTR, ERROR);
    
    if (arg->str[arg->symb_i] == ';')
    {
        while (arg->symb_i < arg->n_symbs && arg->str[arg->symb_i] != '\n')
            arg->symb_i++;
        arg->symb_i++;
    }

    return NO_ERRORS;
}

int Skip_Spaces (struct Argument *arg, Skip_Modes mode)
{
    MY_ASSERT (arg, "struct Argument *arg", NULL_PTR, ERROR);
    
    while (arg->symb_i < arg->n_symbs    && isspace (arg->str[arg->symb_i]) && 
           arg->str[arg->symb_i] != '\n' && arg->str[arg->symb_i] != '\r')
        arg->symb_i++;

    if (mode == MOL && (arg->str[arg->symb_i] == '\n' || arg->str[arg->symb_i] == '\r'))
    {
        Show_Error (arg, "Incomplete line");
        MY_ASSERT (false, "Skip_Modes mode", LINE_ERR, ERROR);
    }
    else if (mode == EOL && arg->symb_i < arg->n_symbs && !isspace (arg->str[arg->symb_i]))
    {
        Show_Error (arg, "Unexpected symbols");
        MY_ASSERT (false, "Skip_Modes mode", LINE_ERR, ERROR);
    } 

    return NO_ERRORS;
}
