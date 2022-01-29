#ifndef ASSEMBLER_H_INCLUDED
#define ASSEMBLER_H_INCLUDED

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <ctype.h>
#include <math.h>

const int MAX_NAME_SIZE = 32;

//REALTED TO LEXER
//******************************************************************************************************************************************
enum Types
{
    CMD,
    BRACKET,
    REG,
    PLUS,
    NUM,
    LBL,
    JMP_ARG
};

union Value
{
    int    cmd_num;
    int    bracket;
    int    reg_name;
    int    plus;
    double number;
    char label[MAX_NAME_SIZE];
    char jmp_arg[MAX_NAME_SIZE];
};

struct Token
{
    enum  Types type;
    union Value value;
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

enum Skip_Modes
{
    EOL,
    MOL
};
struct Token *Read_Asm (int *n_tokens, const char *cmd_file_name);
const char   *Show_CMD_Name (const int cmd_n);
struct Token *Lexer         (const char *str, const long n_symbs, int *n_tokens);

int Process_Line   (struct Argument *arg);
int Process_Name   (struct Argument *arg);
int Check_Name     (struct Argument *arg, const char *name_arr);
int Check_N_Args   (struct Argument *arg, const int cmd_n);
int Process_Arg    (struct Argument *arg);
int Get_Brackets   (struct Argument *arg);
int Get_Inside     (struct Argument *arg);
int Get_Number     (struct Argument *arg);
int Get_Reg        (struct Argument *arg);
int Add_Token_     (struct Argument *arg, Types type, int parmN, ...);
int Skip_Spaces    (struct Argument *arg, Skip_Modes mode);
int Skip_Comments  (struct Argument *arg);
int Show_Error     (struct Argument *arg, const char *err_description);
int Require        (struct Argument *arg, const char symb);

int Check_If_Jump     (const int cmd_num);
int Check_If_Push_Pop (const int pp_num);
//******************************************************************************************************************************************

//RELATED TO PARSER
//******************************************************************************************************************************************
struct Label
{
    char name[MAX_NAME_SIZE];
    int  next_cmd_ip;
};

int  Make_Binary        (const struct Token *token_arr, const int n_tokens);
char *First_Passing     (const struct Token *token_arr, const int n_tokens, struct Label *label_arr, int *n_labels, size_t *code_size);
int  Check_Equal_Labels (const struct Label *label_arr, const int n_labels);
int  Second_Passing     (const struct Token *token_arr, const int n_tokens, const struct Label *label_arr, const int n_labels, char *code);
int  Find_Label         (const struct Label *label_arr, const int n_labels, const char *label_name);
int  Check_If_Pop       (const int cmd_num);
//******************************************************************************************************************************************

#endif