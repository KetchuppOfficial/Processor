#include "Assembler.h"
#include "../Log_File.h"

const char *BINARY_FILE = "../Binary.bin";

int main (int argc, char *argv[])
{
    OPEN_LOG_FILE;

    int n_tokens = 0;
    struct Token *token_arr = Read_Asm (&n_tokens, argv[1]);
    MY_ASSERT(token_arr != NULL, "Read_Asm ()", FUNC_ERROR, ERROR);

    if (Make_Binary (token_arr, n_tokens) == ERROR)
        MY_ASSERT (false, "Make_Binary ()", FUNC_ERROR, ERROR);
        
    free (token_arr);
    
    return 0;
}