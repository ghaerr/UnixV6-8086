/*
    interrupt stack frame, total 12 words
    6   Flags (High memory)
    5   CS
    4   IP
    3   BP
    2   SI
    1   DI
    0   AX    u_ar0
   -1   BX
   -2   CX
   -3   DX
   -4   ES
   -5   DS    (Low memory)
*/
struct ctx
{
    unsigned    ds;     /* 0 */
    unsigned    es;
    unsigned    dx;
    unsigned    cx;
    unsigned    bx;
    unsigned    ax;
    unsigned    di;
    unsigned    si;
    unsigned    bp;
    unsigned    ip;
    unsigned    cs;     /* 20 */
    unsigned    flag;   /* 22 */
    /* not in stack frame */
    unsigned    ss;     /* 24 */
    unsigned    sp;     /* 26 */
};

/* register offsets in struct ctx */
#define R_DS    0
#define R_ES    2
#define R_DX    4
#define R_CX    6
#define R_BX    8
#define R_AX    10
#define R_DI    12
#define R_SI    14
#define R_BP    16
#define R_IP    18
#define R_CS    20
#define R_FLAG  22
#define R_SS    24
#define R_SP    26

/*
 * Location of the users' stored
 * registers relative to R0.
 * Usage is u.u_ar0[XX].
 */
#define R0  (0)
#define R1  (1)
#define R2  (2)
#define R3  (3)
