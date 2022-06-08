DEFCMD_(0,  hlt,  0, return NO_ERRORS; );

DEFCMD_(1,  call,  1,
do
{
    PUSH_F (ip + 2);
    ip = code_arr[ip + 1];
}
while (0);
);

DEFCMD_(2,  jmp,  1,
do
{
    ip = code_arr[ip + 1];
}
while (0);
);

DEFCMD_(3,  jae,  1, 
do
{
    JUMP (">=");
}
while (0);
);

DEFCMD_(4,  ja,  1, 
do
{
    JUMP (">");
}
while (0);
);

DEFCMD_(5,  jbe,  1, 
do
{
    JUMP ("<=");
}
while (0);
);

DEFCMD_(6,  jb,  1, 
do
{
    JUMP ("<");
}
while (0);
);

DEFCMD_(7,  je,  1, 
do
{
    JUMP ("==");
}
while (0);
);

DEFCMD_(8,  jne,  1, 
do
{
    JUMP ("!=");
}
while (0);
);

DEFCMD_(9, ret, 0,
do
{
    double a = 0.0;
    POP_F(&a);
    ip = (int)a;
}
while (0);
);

DEFCMD_(10,  in,   0,
do
{
    double a = 0.0;
    Get_Double (&a);
    PUSH(a);
    ip++;
}
while (0);
);

DEFCMD_(11,  out,  0,
do
{
    double a = 0.0;
    POP(&a);
    printf ("Popped number: %g\n", a);
    ip++;
}
while (0);
);

DEFCMD_(12, push, 1,
do
{
    ip++;
    if (RAM_REG_NUM)
        PUSH_RAM_REG_NUM;
    if (RAM_REG)
        PUSH_RAM_REG;
    if (RAM_NUM)
        PUSH_RAM_NUM;
    if (REG_)
        PUSH_REG;
    if (NUM_)
        PUSH_NUM;
}
while (0);
);

DEFCMD_(13, pop,  1,
do
{
    ip++;
    if (RAM_REG_NUM)
        POP_RAM_REG_NUM;
    if (RAM_REG)
        POP_RAM_REG;
    if (RAM_NUM)
        POP_RAM_NUM;
    if (REG_)
        POP_REG;
    if (NOTHING)
    {
        POP (NULL);
        ip += 3;
    }
}
while (0);
);

DEFCMD_(14, add,  0, 
do
{
    Arithmetic ( '+' );
}
while (0);
);

DEFCMD_(15, sub,  0,
do
{
    Arithmetic ( '-' ); 
}
while (0);
);

DEFCMD_(16, mul,  0,
do
{
    Arithmetic ( '*' );
}
while (0);
);

DEFCMD_(17, dvd,  0,
do
{
    Arithmetic ( '/' );
}
while (0);
);

DEFCMD_(18, sqrt, 0,
do
{
    double a = 0.0;
    POP (&a);
    a = sqrt (a);
    PUSH (a);
    ip++;
}
while (0);
);

DEFCMD_(19, print_roots, 1,
do
{
    int n_roots = (int)*(double *)(code_arr + ip + 1);
    
    if (n_roots == -1)
        printf ("The equation has infinite number of roots\n");
    else if (n_roots == 0)
        printf ("The equation has no roots\n");
    else if (n_roots == 1)
    {
        double a = 0.0;
        POP (&a);
        printf ("The root: %f\n", a);
    }

    ip += 1 + sizeof (double);
}
while (0);
);

DEFCMD_(20, write_circle, 1, 
do
{
    Write_Circle (proc, (int)*(double *)(code_arr + ip + 1));
    ip += 1 + sizeof (double);
}
while (0);
);
