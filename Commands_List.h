DEFCMD_(0,  hlt,  0,
{
    return NO_ERRORS;
});

DEFCMD_(1,  call,  1,
{
    PUSH_F (NEXT_CMD);
    shift = VALUE;
});

DEFCMD_(2,  jae,  1, {JUMP ( >= ); });

DEFCMD_(3,  ja,   1, {JUMP ( > ); });

DEFCMD_(4,  jbe,  1, {JUMP ( <= ); });

DEFCMD_(5,  jb,   1, {JUMP ( < ); });

DEFCMD_(6,  je,   1, {JUMP ( == ); });

DEFCMD_(7,  jne,  1, {JUMP ( != ); });

DEFCMD_(8, ret, 0,
{
    int a = 0;
    POP_F(&a);
    shift = a;
});

DEFCMD_(9,  in,   0,
{
    int a = 0;
    Get_Int (&a);
    PUSH(a);

    shift += BYTE;
});

DEFCMD_(10,  out,  0,
{
    int a = 0;
    POP(&a);
    printf ("Popped number: %d\n", a);

    shift += BYTE;
});

DEFCMD_(11, push, 1,
{
    MEGA_STACK (PUSH (PUSH_RAM_REG),
                PUSH (PUSH_RAM_N_REG),
                PUSH (PUSH_N_RAM_REG),
                PUSH (PUSH_N_RAM_N_REG));
});

DEFCMD_(12, pop,  1,
{
    MEGA_STACK (POP (POP_RAM_REG),
                POP (POP_RAM_N_REG),
                POP (POP_N_RAM_REG),
                POP_N_RAM_N_REG);
});

DEFCMD_(13, add,  0, { Arithmetic ( '+' ); });

DEFCMD_(14, sub,  0, { Arithmetic ( '-' ); });

DEFCMD_(15, mul,  0, { Arithmetic ( '*' ); });

DEFCMD_(16, dvd,  0, { Arithmetic ( '/' ); });
