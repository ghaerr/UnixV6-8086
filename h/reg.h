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
    unsigned    ds;
    unsigned    es;
    unsigned    dx;
    unsigned    cx;
    unsigned    bx;
    unsigned    ax;
    unsigned    di;
    unsigned    si;
    unsigned    bp;
    unsigned    ip;
    unsigned    cs;
    unsigned    flag;
    /* not in stack frame */
    unsigned    ss;
    unsigned    sp;
};

/*
 * Location of the users' stored
 * registers relative to R0.
 * Usage is u.u_ar0[XX].
 */
#define R0  (0)
#define R1  (1)
#define R2  (2)
#define R3  (3)

#define R_DI (1)
#define R_SI (2)
#define R_BP (3)
#define R_IP (4)
#define R_CS (5)
#define R_FLAGS (6)

#define R_BX (-1)
#define R_CX (-2)
#define R_DX (-3)
#define R_ES (-4)
#define R_DS (-5)
