#include "../include/Processor.h"

int Check_Argc (const int argc, const int expected)
{
    return (argc == expected) ? 0 : 1;
}

int main (int argc, char *argv[])
{
    #ifdef DEBUG
    Open_Log_File ("Processor");
    #endif

    MY_ASSERT (Check_Argc (argc, 2) == 0, "int argc", UNEXP_VAL, ERROR);

    struct Processor *proc = Processor_Ctor ();
    
    long max_ip = 0;
    char *code = Make_File_Buffer (argv[1], &max_ip);
    MY_ASSERT (code != NULL, "Make_File_Buffer ()", FUNC_ERROR, ERROR);

    #ifdef DEBUG
    int PC_status = Process_Code (code, max_ip, proc);
    #else
    Process_Code (code, max_ip, proc);
    #endif

    MY_ASSERT (PC_status != ERROR, "Process_Code ()", FUNC_ERROR, ERROR);

    Processor_Dtor (proc);
    
    return 0;
}
