#include "Processor.h"
#include "../My_Lib/My_Lib.h"

int Check_Argc (const int argc, const int expected)
{
    return (argc == expected) ? 0 : 1;
}

int main (int argc, char *argv[])
{
    #ifdef DEBUG
    OPEN_LOG_FILE;
    #endif

    MY_ASSERT (Check_Argc (argc, 2) == 0, "int argc", UNEXP_VAL, ERROR);

    struct Processor pentium = {};
    Stack_Ctor (&pentium.stack);
    Stack_Ctor (&pentium.call_stack);
    
    int max_ip = 0;
    char *code = Get_Code (argv[1], &max_ip);
    MY_ASSERT (code != NULL, "Get_Code ()", FUNC_ERROR, ERROR);

    #ifdef DEBUG
    int PC_status = Process_Code (code, max_ip, &pentium);
    #else
    Process_Code (code, max_ip, &pentium);
    #endif

    for (long long i = 0; i < 100000000; i++)
        Process_Code (code, max_ip, &pentium);

    MY_ASSERT (PC_status != ERROR, "Process_Code ()", FUNC_ERROR, ERROR);

    Stack_Dtor (&pentium.stack);
    Stack_Dtor (&pentium.call_stack);
    
    return 0;
}
