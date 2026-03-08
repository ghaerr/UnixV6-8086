#include "os.h"

/*
 *  Memory special file
 *  minor device 0 is physical memory
 *  minor device 1 is kernel memory
 *  minor device 2 is EOF/RATHOLE
 */

int mmread(int dev)
{
    register c, bn, on;

    if(minor(dev) == 2)
        return 0;
    do {
        if(minor(dev) == 1)
            c = *(char *)u.u_offset[1];
        else {
            bn = lshift(u.u_offset, -4);
            on = u.u_offset[1] & 0x0f;
            c = peekb(on, bn);
        }
    } while(u.u_error==0 && passc(c)>=0);
}

int mmwrite(int dev)
{
    register c, bn, on;

    if(minor(dev) == 2) {
        c = u.u_count;
        u.u_count = 0;
        u.u_base += c;
        dpadd(u.u_offset, c);
        return;
    }
    for(;;) {
        if ((c=cpass())<0 || u.u_error!=0)
            break;
        if(minor(dev) == 1)
            *(char *)u.u_offset[1] = c;
        else {
            bn = lshift(u.u_offset, -4);
            on = u.u_offset[1] & 0x0f;
            pokeb(on, bn, c);
        }
    }
}

int mmsgtty(int dev, int *v)
{
    return 0;
}
