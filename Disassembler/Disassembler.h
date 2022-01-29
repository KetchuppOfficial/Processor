#ifndef DISASSEMBLER_H_INCLUDED
#define DISASSEMBLER_H_INCLUDED

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *Get_Code         (int *max_ip);
int  Disassemble       (const char *code, const int max_ip);
char *First_Passing (const char *code_arr, const int max_ip, int *n_labels);
int  Check_N_Args      (const int cmd_n);
int  Check_If_Jump     (const int cmd_num);
int  Check_If_Push_Pop (const int pp_num);
int  Second_Passing    (const char *code_arr, const int max_ip, FILE *file_ptr, const char *label_arr, const int n_labels);
const char *Show_CMD_Name    (const int cmd_n);

#endif