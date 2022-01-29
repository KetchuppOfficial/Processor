#include "Disassembler.h"
#include "../Log_File.h"

const char *BINARY_FILE = "../Binary.bin";
const char *DISASM_FILE = "./Disassembled.txt";

int main (void)
{
    OPEN_LOG_FILE;

    int max_ip = 0;
    char *code = Get_Code (&max_ip);
    MY_ASSERT (code != NULL, "Get_Code ()", FUNC_ERROR, ERROR);

    if (Disassemble (code, max_ip) == ERROR)
        MY_ASSERT (false, "Disassemble ()", FUNC_ERROR, ERROR);

    free (code);

    return 0;
}