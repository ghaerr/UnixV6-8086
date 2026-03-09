#include "os.h"

struct user u;
struct proc proc[NPROC];
struct inode *rootdir;

int core_cs = LOADSEG;
int core_ds = LOADSEG;
int core_spl;

int mpid;
char runin;
char runout;
char runrun;
char curpri;
int rootdev = 0;
int swapdev = 0;
int swplo = 4000;
int nswap = 872;
int updlock = 0;

int execnt;
int lbolt;
int time[2];
int tout[2];
int nchrdev;

struct mount mount[NMOUNT];
struct inode inode[NINODE];
char canonb[CANBSIZ];
int coremap[CMAPSIZ];
int swapmap[SMAPSIZ];
struct callo callout[NCALL];

extern struct devtab rktab;

struct bdevsw bdevsw[] = {
    { nulldev, nulldev, rkstrategy, &rktab },
    { NULL, NULL, NULL, NULL},
};

struct cdevsw cdevsw[] = {
    { klopen, klclose, klread, klwrite, klsgtty },
    { nulldev, nulldev, mmread, mmwrite, mmsgtty },
    { NULL, NULL, NULL, NULL, NULL }
};

/*
 * Icode is the octal bootstrap
 * program executed in user mode
 * to bring up the system.
 *
 * Disassembly:
 * CS:
 * 0000        B8 07 00         MOV AX, 0007h  ; argv
 * 0003        50               PUSH AX
 * 0004        B8 02 00         MOV AX, 0002h  ; prog
 * 0007        50               PUSH AX
 * 0008        BA 0B 00         MOV DX, 000Bh  ; sys_exec
 * 000B        CD 81            INT 81h        ; syscall exec(prog, argv)
 * DS:
 * 0000        00 00                           ; disallow NULL pointers to data
 * 0002  prog: 69 6E 69 74 00   DB 'init\0'    ; program to exec
 * 0007  argv: 02 00            DW av          ; argv[] array
 * 0009        00 00            DW 0           ; NULL
 */
char icode[] = {
    0xB8, 0x07, 0x00, 0x50, 0xB8, 0x02, 0x00, 0x50,
    0xBA, 0x0B, 0x00, 0xCD, 0x81
};
char idata[] = {
    0x00, 0x00, 0x69, 0x6E, 0x69, 0x74, 0x00, 0x02,
    0x00, 0x00, 0x00
};

void main()
{
    pc_init();

    mfree(coremap, 128, USPACE);
    mfree(swapmap, nswap, swplo);

    /*
     * set up system process
     */
    proc[0].p_addr = core_ds/(PAGESIZ/16);
    proc[0].p_size = PSIZE;
    proc[0].p_stat = SRUN;
    proc[0].p_flag |= SLOAD|SSYS;
    u.u_procp = &proc[0];

    cinit();
    binit();
    iinit();

    rootdir = iget(rootdev, ROOTINO);
    rootdir->i_flag &= ~ILOCK;
    u.u_cdir = iget(rootdev, ROOTINO);
    u.u_cdir->i_flag &= ~ILOCK;

    printk("Unix Ready.\n");

    /*
     * make init process
     * enter scheduling loop
     * with system process
     */

    if(newproc()) {
        copyout((uint)icode, sizeof(icode), 0, (u.u_procp->p_addr+DSIZE)*(PAGESIZ/16));
        copyout((uint)idata, sizeof(idata), 0, u.u_procp->p_addr*(PAGESIZ/16));
        move_to_user_mode(u.u_procp->p_addr*(PAGESIZ/16));
        /*
         * Return goes to loc. 0 of user init
         * code just copied out.
         */
        return;
    }
    sched();
}
