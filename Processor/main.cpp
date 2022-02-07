#include "Processor.h"
#include "../Log_File.h"

const char *BINARY_FILE = "Binary.bin";

int main (void)
{
    OPEN_LOG_FILE;

    struct Processor pentium = {};
    Stack_Ctor (&pentium.stack);
    Stack_Ctor (&pentium.call_stack);
    
    int max_ip = 0;
    char *code = Get_Code (&max_ip);
    MY_ASSERT (code != NULL, "Get_Code ()", FUNC_ERROR, ERROR);

    if (Process_Code (code, max_ip, &pentium) == ERROR)
        MY_ASSERT (false, "Process_Code ()", FUNC_ERROR, ERROR);

    free (code);
    Stack_Dtor (&pentium.stack);
    Stack_Dtor (&pentium.call_stack);
    
    return 0;
}