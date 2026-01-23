#include "os.h"

/*
 * Swap out process p.
 * The ff flag causes its core to be freed--
 * it may be off when called to create an image for a
 * child process in newproc.
 * Os is the old size of the data area of the process,
 * and is supplied during core expansion swaps.
 *
 * panic: out of swap space
 * panic: swap error -- IO error
 */
void xswap(struct proc *p, int ff, uint a)
{
    register struct proc *rp;
    int os;

    rp = p;
    os = rp->p_size;

    xccdec(rp->p_textp);
    rp->p_flag |= SLOCK;
    if(swap(a, rp->p_addr, os, 0))
        panic("swap error");
    if(ff)
        mfree(coremap, os, rp->p_addr);
    rp->p_addr = a;
    rp->p_flag &= ~(SLOAD|SLOCK);
    rp->p_time = 0;
    if(runout) {
        runout = 0;
        wakeup(&runout);
    }
}

/*
 * relinquish use of the shared text segment
 * of a process.
 */
void xfree(void)
{
    struct text *xp;
    struct inode *ip;

    if((xp=u.u_procp->p_textp) != NULL) {
        u.u_procp->p_textp = NULL;
        xccdec(xp);
        if(--xp->x_count == 0) {
            ip = xp->x_iptr;
            if((ip->i_mode&ISVTX) == 0) {
                xp->x_iptr = NULL;
                mfree(swapmap, (xp->x_size+7)/8, xp->x_daddr);
                ip->i_flag &= ~ITEXT;
                iput(ip);
            }
        }
    }
}

/*
 * Decrement the in-core usage count of a shared text segment.
 * When it drops to zero, free the core space.
 */
void xccdec(struct text *xp)
{
    register struct text *rp;

    if((rp=xp)!=NULL && rp->x_ccount!=0)
        if(--rp->x_ccount == 0)
            mfree(coremap, rp->x_size, rp->x_caddr);
}
