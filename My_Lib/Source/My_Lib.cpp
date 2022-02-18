#include "../My_Lib.h"

//RELATED TO LOG FILE
//***************************************************************************************
FILE *LOG_FILE = NULL;

static const char *ERROR_DESCRIPTIONS[] =
{
    "Undefined behavior",
    "Error in called function",

    "Impossible to open log file",
    "Impossible to open file",
    "Impossible to close file",
    "Empty file",
    "Incorrect file",
    "The number of written symbols differs from the expected one",
    "Error in work of function fseek ()",
    "Error in work of function ftell ()",
    "Error in work of function _stat()",

    "Expected positive value",
    "Unexpected value",
    "Unexpected symbol",
    "Expected integer number",
    "Pointer is NULL",
    "Pointer is not NULL",
    "Impossible to allocate enough memory",
    "0^0 is undefined",
    "Dividing by 0 is undefined",

    "Incorrect label",
    "Empty label",
    "Equal labels have been found",
    "The number of jumps cannot be greater than the number of labels",
    "There is no label for a jump",
    "Label should be a positive integer number",
    "Label cannot be the last instruction",

    "Undefined command",
    "Incorrect argument",
    "No digits after decimal point",
    "Too few arguments",
    "Too many arguments",
    "Incomplete line",
    "Second brace expected",
    "Two brackets in a row",

     "The radius of a circle has to be in range from 1 to 100",

    "Constructing error",
    "Pushing error",
    "Popping error",
    "Destructing error",
    "It's forbidden to use uninitialized stack",
    "It's forbidden to pop stack which size is 0",
    "It's forbidden to initialize stack more than once",
    "It's forbidden to resize down stack which capacity is minimal or less than minimal",
    "Resizing error",
    "Left stack canary changed its value",
    "Right stack canary changed its value",
    "Left data canary changed its value",
    "Right data canary changed its value",
    "Hash changed its value unexpectedly",

    "It's forbidden to divide by zero",

    "The next pointer of the last node should be marked",
    "The prev pointer of the first node should be marked",
    "List underflow",
    "List overflow",
    "It's forbidden to add element after position that is not between 0 and current size",
    "It's forbidden to delete element after position that is not between 0 and current size",
    "It's forbidden to insert element after free node",
    "It's forbidden to delete element after free node",
    "It's forbidden to add elements before something in empty list",
    "It's forbidden to use uninitialized list",
    "There is no such logical number in list",
    "The \"prev\" of head has to be equal to 0",
    "The \"next\" of tail has to be equal to 0",
    "The structure of list has been damaged. The order of elements is incorrect",
    "The structure of list of free nodes is damaged",

    "This node already has left son",
    "This node already has right son"
};

enum ERROR_TYPES
{
    UNDEF_BEH = 0,
    FUNC_ERROR,

    LOG_NOT_OPEN,
    OPEN_ERR,
    CLOSE_ERR,
    EMP_FILE,
    INCORR_FILE,
    FILE_WRT,
    FSEEK_ERR,
    FTELL_ERR,
    STAT_ERR,

    POS_VAL,
    UNEXP_VAL,
    UNEXP_SYMB,
    INT_EXPECTED,
    NULL_PTR,
    NOT_NULL_PTR,
    NE_MEM,
    NULL_POW_NULL,
    DIV_BY_NULL,

    INCORR_LBL,
    EMP_LBL,
    EQUAL_LBLS,
    TF_LBLS,
    UNKNOWN_JMP,
    POS_LBL,
    END_LBL,

    UNDEF_CMD,
    INCORR_ARG,
    WRONG_DECIMAL,
    TF_ARGS,
    TM_ARGS,
    LINE_ERR,
    SCND_BRACE,
    TWO_BRACK,

    RADIUS,

    CTOR_ERR,
    PUSH_ERR,
    POP_ERR,
    DTOR_ERR,
    UNINIT_STACK,
    ZERO_POP,
    MULT_CTOR,
    BAD_RESIZE,
    RSZ_ERR,
    L_S_CANARY_CG,
    R_S_CANARY_CG,
    L_D_CANARY_CG,
    R_D_CANARY_CG,
    HASH_CHANGE,

    ZERO_DIV,

    END_I_ERR,
    BEG_I_ERR,
    LIST_UF,
    LIST_OF,
    INV_POS,
    INV_DEL,
    FREE_INSERT,
    FREE_DEL,
    BEFORE_ZERO,
    UNINIT_LIST,
    NO_NUM,
    PREV_HEAD,
    NEXT_TAIL,
    LIST_DAM,
    FREE_DAM,

    HAS_L_SON,
    HAS_R_SON
};

enum ERRORS
{
    NO_ERRORS = -101,
    ERROR,
};

int Open_Log_File (void)
{
    LOG_FILE = fopen ("log_file.log", "wb");

    if (!LOG_FILE)
    {
        LOG_FILE = stderr;
        MY_ASSERT (false, "LOG_FILE", LOG_NOT_OPEN, LOG_NOT_OPEN);
    }

    fprintf (LOG_FILE, "**********************************\n");
    fprintf (LOG_FILE, "*        THIS IS LOG_FILE        *\n");
    fprintf (LOG_FILE, "**********************************\n\n");

    atexit (Close_Log_File);

    return NO_ERRORS;
}

void Close_Log_File (void)
{
    if (fclose (LOG_FILE) == 0)
        printf ("Log file is closed successfully\n");
    else
        printf ("Log file is NOT closed successfully\n");
}

int My_Assert (bool condition, const char *const file, const int line,
               const char *const fun_name, const char *const var_name, const int err_name)
{
    if (!LOG_FILE || !fun_name || !var_name || err_name < 0 || line <= 0)
        return ERROR;

    if (!condition)
    {
        fprintf (LOG_FILE, "File: %s\n",                                   file);
        fprintf (LOG_FILE, "Line: %d\n",                                   line);
        fprintf (LOG_FILE, "Function with error: %s\n",                    fun_name);
        fprintf (LOG_FILE, "Variable or function that caused error: %s\n", var_name);
        fprintf (LOG_FILE, "Error description: %s\n\n",                    ERROR_DESCRIPTIONS[err_name]);

        return 1;
    }

    return 0;
}
//***************************************************************************************

//RELATED TO READING FROM FILE
//***************************************************************************************
FILE *Open_File (const char *const file_name, const char *const mode)
{
    MY_ASSERT (file_name, "const char *const file_name", NULL_PTR, NULL);
    MY_ASSERT (mode,      "const char *const mode",      NULL_PTR, NULL);

    FILE *file_ptr = fopen (file_name, mode);

    MY_ASSERT (file_ptr, "FILE *file_ptr", OPEN_ERR, NULL);

    return file_ptr;
}

int Close_File (FILE *file_ptr, const char *const file_name)
{
    MY_ASSERT (file_ptr,  "FILE *file_ptr",        NULL_PTR, ERROR);
    MY_ASSERT (file_name, "const char *file_name", NULL_PTR, ERROR);

    MY_ASSERT (fclose (file_ptr) == 0, "fclose ()", CLOSE_ERR, ERROR);

    return NO_ERRORS;
}

long Define_File_Size (FILE *file_ptr)
{
    MY_ASSERT (file_ptr, "FILE *file_ptr", NULL_PTR, ERROR);

    long start_pos = ftell (file_ptr);

    if (fseek (file_ptr, 0L, SEEK_END))
        MY_ASSERT (0, "fseek ()", FSEEK_ERR, ERROR);

    long n_symbs = ftell (file_ptr);
    MY_ASSERT (n_symbs != -1L, "long n_symbs", FTELL_ERR, ERROR);
    MY_ASSERT (n_symbs > 0L,   "long n_symbs", EMP_FILE, ERROR);

    if (fseek (file_ptr, start_pos, SEEK_SET))
        MY_ASSERT (false, "fseek ()", FSEEK_ERR, ERROR);

    return n_symbs;
}

char *Make_Buffer (FILE *file_ptr, const long n_symbs)
{
    MY_ASSERT (file_ptr,     "FILE *file_ptr",     NULL_PTR, NULL);
    MY_ASSERT (n_symbs > 0L, "const long n_symbs", POS_VAL,  NULL);

    char *buffer = (char *)calloc (n_symbs + 1L, sizeof (char));
    MY_ASSERT (buffer, "char *buffer", NE_MEM, NULL);

    size_t read_symbs = fread (buffer, sizeof (char), n_symbs, file_ptr);

    MY_ASSERT (read_symbs == (size_t)n_symbs, "size_t read_symbs", FILE_WRT, NULL);

    return buffer;
}
//***************************************************************************************
