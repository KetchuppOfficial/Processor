#include "../Assembler.h"

int Check_Argc (const int argc, const int expected)
{
    return (argc == expected) ? 0 : 1;
}

int main (int argc, char *argv[])
{
    OPEN_LOG_FILE;

    MY_ASSERT (Check_Argc (argc, 3) == 0, "int argc", UNEXP_VAL, ERROR);

    int n_tokens = 0;
    struct Token *token_arr = Read_Asm (&n_tokens, argv[1]);
    MY_ASSERT(token_arr != NULL, "Read_Asm ()", FUNC_ERROR, ERROR);

    if (Make_Binary (token_arr, n_tokens, argv[2]) == ERROR)
        MY_ASSERT (false, "Make_Binary ()", FUNC_ERROR, ERROR);
        
    free (token_arr);
    
    return 0;
}
