#include "../Disassembler.h"

int Check_Argc (const int argc, const int expected)
{
    return (argc == expected) ? 0 : 1;
}

int main (int argc, char *argv[])
{
    Open_Log_File ("Disassembler");

    MY_ASSERT (Check_Argc (argc, 3) == 0, "int argc", UNEXP_VAL, ERROR);

    if (Disassemble (argv[1], argv[2]) == ERROR)
        MY_ASSERT (false, "Disassemble ()", FUNC_ERROR, ERROR);

    return 0;
}
    