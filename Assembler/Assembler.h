#ifndef ASSEMBLER_H_INCLUDED
#define ASSEMBLER_H_INCLUDED

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <ctype.h>
#include <math.h>
#include <assert.h>

#include "../My_Lib/My_Lib.h"

#define MAX_NAME_SIZE 32

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
    char   label  [MAX_NAME_SIZE];
    char   jmp_arg[MAX_NAME_SIZE];
};

struct Token
{
    enum  Types type;
    union Value value;
};

struct Token *Read_Asm    (int *n_tokens, const char *cmd_file_name);
int           Make_Binary (const struct Token *token_arr, const int n_tokens, const char *output_name);
//******************************************************************************************************************************************

#endif