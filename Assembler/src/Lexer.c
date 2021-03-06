#include "../Assembler.h"

enum Skip_Modes
{
    EOL,
    MOL
};

struct Argument
{
    const char *const str;
    const long        n_symbs;
    long              symb_i;
    struct Token      *token_arr;
    int               token_i;
    int               line;
};

static int Skip_Comments (struct Argument *arg)
{
    MY_ASSERT (arg, "struct Argument *arg", NULL_PTR, ERROR);
    
    if (arg->str[arg->symb_i] == ';')
    {
        while (arg->symb_i < arg->n_symbs && arg->str[arg->symb_i] != '\n')
            arg->symb_i++;
    }

    return NO_ERRORS;
}

static int Show_Error (struct Argument *arg, const char *err_description)
{
    MY_ASSERT (arg,             "struct Argument *arg",        NULL_PTR, ERROR);
    MY_ASSERT (err_description, "const char *err_description", NULL_PTR, ERROR);

    long line_beginning = arg->symb_i;

    while (line_beginning >= 0 && arg->str[line_beginning] != '\n')
        line_beginning--;
    line_beginning++;

    long shift = arg->symb_i - line_beginning;
    
    fprintf (stderr, "\n***************************************************************************************************\n");
    fprintf (stderr, "LINE %d:\n", arg->line);
    for (long i = 0; line_beginning + i < arg->n_symbs && arg->str[line_beginning + i] != '\r' && arg->str[line_beginning + i] != '\n'; i++)
        fprintf (stderr, "%c", arg->str[line_beginning + i]);
    fprintf (stderr, "\n");
    
    for (long i = 0; i < shift; i++)
    {
        if (arg->str[line_beginning + i] == '\t')
            fprintf (stderr, "\t");
        else
            fprintf (stderr, " ");
    }
    fprintf (stderr, "^~~~~~~~~~~ %s", err_description);

    fprintf (stderr, "\n***************************************************************************************************\n");

    return NO_ERRORS;
}

static int Skip_Spaces (struct Argument *arg, enum Skip_Modes mode)
{
    MY_ASSERT (arg, "struct Argument *arg", NULL_PTR, ERROR);
    
    while (arg->symb_i < arg->n_symbs    && isspace (arg->str[arg->symb_i]) && 
           arg->str[arg->symb_i] != '\n' && arg->str[arg->symb_i] != '\r')
        arg->symb_i++;

    if (mode == MOL && (arg->str[arg->symb_i] == '\n' || arg->str[arg->symb_i] == '\r'))
    {
        if (arg->token_arr[arg->token_i - 1].value.cmd_num != 13)
        {
            Show_Error (arg, "Incomplete line");
            MY_ASSERT (false, "arg->str[arg->symb_i]", UNEXP_SYMB, ERROR);
        }
    }
    else if (mode == EOL && arg->symb_i < arg->n_symbs && !isspace (arg->str[arg->symb_i]))
    {
        Show_Error (arg, "Unexpected symbols");
        MY_ASSERT (false, "arg->str[arg->symb_i]", UNEXP_SYMB, ERROR);
    } 

    return NO_ERRORS;
}

static int Add_Token_ (struct Argument *arg, enum Types type, int parmN, ...)
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

#define Add_Token(arg, type, token)                             \
do                                                              \
{                                                               \
    if (Add_Token_ (arg, type, 1, token) == ERROR)              \
        MY_ASSERT (false, "Add_Token_ ()", FUNC_ERROR, ERROR);  \
}                                                               \
while (0)

static int Get_Number (struct Argument *arg)
{
    MY_ASSERT (arg, "struct Argument *arg", NULL_PTR, ERROR);

    double num = 0.0;
    bool is_negative = false;
    if (arg->str[arg->symb_i] == '-')
    {
        is_negative = true;
        arg->symb_i++;
    }

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
            MY_ASSERT (false, "const char *old_str", UNEXP_SYMB, ERROR);
        }
    }

    if (is_negative)
        num = -num;

    Add_Token (arg, NUM, num);

    return NO_ERRORS;
}

static int Get_Reg (struct Argument *arg)
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

static int Get_Inside (struct Argument *arg)
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

static int Require (struct Argument *arg, const char symb)
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

static int Get_Brackets (struct Argument *arg)
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
    else if (isdigit (arg->str[arg->symb_i]) || arg->str[arg->symb_i] == '-')
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

int Process_Arg (struct Argument *arg)
{
    MY_ASSERT (arg, "struct Argument *arg", NULL_PTR, ERROR);
    
    if (Check_If_Jump (arg->token_arr[arg->token_i - 1].value.cmd_num))
    {
        char arg_arr[MAX_NAME_SIZE] = "";

        int i = 0;
        for (i = 0; !isspace (arg->str[arg->symb_i]); i++, arg->symb_i++)
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
#define DEFCMD_(num, name, n_args, code)    \
do                                          \
{                                           \
    if (strcmp (name_arr, #name) == 0)      \
        return num;                         \
}                                           \
while (0)

static int Check_Name (struct Argument *arg, const char *name_arr)
{
    MY_ASSERT (name_arr, "const char *name_arr", NULL_PTR, ERROR);
    MY_ASSERT (arg,      "struct Argument *arg", NULL_PTR, ERROR);
    
    for (int i = 0; ; i++)
    {
        if (i > 0)
        {
            Show_Error (arg, "There is no command with this name");
            MY_ASSERT (false, "const char *name_arr", UNEXP_SYMB, ERROR);
        }

        #include "../../Commands_List.h"
    }
}

#undef DEFCMD_

#define DEFCMD_(num, name, n_args, code)    \
case num:                                   \
    return n_args;

static int Check_N_Args (struct Argument *arg, const int cmd_n)
{    
    MY_ASSERT (arg, "struct Argument *arg", NULL_PTR, ERROR);
        
    switch (cmd_n)
    {
        #include "../../Commands_List.h"

        default:
            Show_Error (arg, "There is no command with this number");
            MY_ASSERT (false, "const int cmd_n", UNEXP_VAL, ERROR);
            break;
    }
}
#undef DEFCMD_

static int Process_Name (struct Argument *arg)
{
    MY_ASSERT (arg, "struct Argument *arg", NULL_PTR, ERROR);

    char name_arr[MAX_NAME_SIZE] = "";

    int cmd_num = 0, n_args = 0;

    int i = 0;
    for (i = 0; isalpha (arg->str[arg->symb_i]) || arg->str[arg->symb_i] == '_'; i++, arg->symb_i++)
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

static int Process_Line (struct Argument *arg)
{
    MY_ASSERT (arg, "struct Argument *arg", NULL_PTR, ERROR);
    
    if (Skip_Spaces (arg, MOL) == ERROR)
        MY_ASSERT (false, "Skip_Spaces ()", FUNC_ERROR, ERROR);

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
    else if (arg->symb_i < arg->n_symbs && !isspace (arg->str[arg->symb_i]))
    {
        Show_Error (arg, "Unexpected symbol");
        MY_ASSERT (false, "Buffer with assembler code", UNEXP_SYMB, ERROR);
    }
    
    return NO_ERRORS;
}

static struct Token *Lexer (const char *str, const long n_symbs, int *n_tokens)
{
    MY_ASSERT (str,         "const char *str",    NULL_PTR, NULL);
    MY_ASSERT (n_symbs > 0, "const long n_symbs", POS_VAL,  NULL);
    MY_ASSERT (n_tokens,    "int *n_toknes",      NULL_PTR, NULL);

    struct Token *token_arr = (struct Token *)calloc (n_symbs, sizeof (struct Token));
    MY_ASSERT (token_arr, "struct Token *token_arr", NE_MEM, NULL);

    struct Argument arg = {str, n_symbs, 0, token_arr, 0, 1};

    while (arg.symb_i < arg.n_symbs)
    {
        while (arg.symb_i < arg.n_symbs && (isspace (arg.str[arg.symb_i]) || arg.str[arg.symb_i] == ';'))
        {
            if (isspace (arg.str[arg.symb_i]))
            {
                if (arg.str[arg.symb_i] == '\n')
                    arg.line++;
                arg.symb_i++;
            }
            else if (arg.str[arg.symb_i] == ';')
                if (Skip_Comments (&arg) == ERROR)
                    MY_ASSERT (false, "Skip_Comments ()", FUNC_ERROR, NULL);
        }
        if (arg.symb_i < arg.n_symbs && Process_Line (&arg) == ERROR)
            MY_ASSERT (false, "Process_Line ()", FUNC_ERROR, NULL);
    }
        
    *n_tokens = arg.token_i;

    return token_arr;
}

struct Token *Read_Asm (int *n_tokens, const char *cmd_file_name)
{
    MY_ASSERT (n_tokens,      "int *n_tokens",             NULL_PTR, NULL);
    MY_ASSERT (cmd_file_name, "const char *cmd_file_name", NULL_PTR, NULL);

    long n_symbs = 0L;
    char *buffer = Make_File_Buffer (cmd_file_name, &n_symbs);

    struct Token *token_arr = Lexer (buffer, n_symbs, n_tokens);
    MY_ASSERT (token_arr != NULL, "Lexer ()", FUNC_ERROR, NULL);

    #ifdef DEBUG
    printf ("\nDEBUG INFO:\n"
            "********************\n");
    for (int i = 0; i < *n_tokens; i++)
    {
        switch (token_arr[i].type)
        {
            case CMD:      printf ("command: %s\n",        Get_CMD_Name (token_arr[i].value.cmd_num));            break;
            case BRACKET:  printf ("bracket: %c\n",                      token_arr[i].value.bracket);             break;
            case REG:      printf ("register name: %cx\n",               token_arr[i].value.reg_name + 'a' - 1);  break;
            case PLUS:     printf ("plus: %c\n",                         token_arr[i].value.plus);                break;
            case NUM:      printf ("number: %f\n",                       token_arr[i].value.number);              break;
            case LBL:      printf ("label: %s\n",                        token_arr[i].value.label);               break;
            case JMP_ARG:  printf ("jump arg: %s\n",                     token_arr[i].value.jmp_arg);             break;

            default:
                printf ("UNKNOWN TYPE\n");
        }
    }
    printf ("********************\n");
    #endif

    free (buffer);

    return token_arr;
}
