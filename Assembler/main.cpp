#include "Assembler.h"
#include "../Log_File.h"

const char *CMD_FILE_NAME = "../Assembler_File.txt";
const char *BINARY_FILE = "../Binary.bin";

int main (void)
{
    OPEN_LOG_FILE;

    int n_tokens = 0;
    struct Token *token_arr = Read_Asm (&n_tokens);
    MY_ASSERT(token_arr != NULL, "Read_Asm ()", FUNC_ERROR, ERROR);

    if (Make_Binary (token_arr, n_tokens) == ERROR)
        MY_ASSERT (false, "Make_Binary ()", FUNC_ERROR, ERROR);
        
    free (token_arr);
    
    return 0;
}