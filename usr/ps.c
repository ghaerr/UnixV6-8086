#include "unix.h"

/*
 *	ps - process status
 *	examine and print certain things about processes
 *  
 *  Not elegant, not safe — but we really
 *  need this tool to see what's running inside.
 */

/* RealXV6 doesn't use nlist, we use getkaddr syscall */
/*
struct {
	char name[8];
	int  type;
	char  *value;
} nl[3];
*/

/* 
 * We need to make sure NPROC and other constants match kernel.
 * They should come from param.h
 */
#include "../h/param.h"
#include "../h/proc.h"

struct proc proc[NPROC];
int	mem;
int	kmem;
int	swap;

int prcom(int i);
void getdev(int swap_dev_num);
int readproc(struct proc *p_proc, uint addr, int len, char *result);
int readword(struct proc *p_proc, uint addr, int *result);

int main()
{
	int i, puid;
    int proc_addr, swap_addr, swap_dev_num;

	if(chdir("/dev") < 0) {
		printf("cannot change to /dev\n");
		exit();
	}
    /* 
     * Replacing nlist logic with sys_getkaddr 
     */
    proc_addr = getkaddr(0);
    swap_addr = getkaddr(1);
	if (proc_addr == 0) {
		printf("No namelist\n");
		exit();
	}
	if ((kmem = open("/dev/kmem", 0)) < 0) {
		printf("No kmem\n");
		exit();
	}
	if ((mem = open("/dev/mem", 0)) < 0) {
		printf("No mem\n");
		exit();
	}
	seek(kmem, swap_addr, 0);
    read(kmem, &swap_dev_num, 2);
	seek(kmem, proc_addr, 0);
	read(kmem, proc, sizeof proc);
	getdev(swap_dev_num);
	printf(" F S UID   PID   PRI ADDR  WCHAN COMMAND\n");
	for (i=0; i<NPROC; i++) {
		if (proc[i].p_stat==0)
			continue;
		puid = proc[i].p_uid & 0377;
		printf("%2x %c%4d", proc[i].p_flag,
			"0SWRIZT"[proc[i].p_stat], puid);
		printf("%6u", proc[i].p_pid);
		printf("%6d%5x", proc[i].p_pri, proc[i].p_addr);
		if (proc[i].p_wchan)
			printf("%7x", proc[i].p_wchan); else
			printf("       ");
		if (proc[i].p_stat==5)
			printf(" <defunct>");
		else
			prcom(i);
		printf("\n");
	}
	exit();
}

struct dentry{ int dir_ino; char dir_n[14]; };

void getdev(int swap_dev_num)
{
	register struct dentry *p;
	register int i;
	int f;
	char dbuf[512];
	struct stat sbuf;

	f = open("/dev", 0);
	if(f < 0) {
		printf("cannot open /dev\n");
		exit();
	}
	swap = -1;

loop:
	i = read(f, dbuf, 512);
	if(i <= 0) {
		close(f);
		if(swap < 0) {
			printf("no swap device\n");
			exit();
		}
		return;
	}
	while(i < 512)
		dbuf[i++] = 0;
	for(p = (struct dentry *)dbuf; p < (struct dentry *)(dbuf+512); p++) {
		if(p->dir_ino == 0)
			continue;
		if(swap >= 0)
			continue;
		if(stat(p->dir_n, &sbuf) < 0)
			continue;
		if((sbuf.s_flags & 060000) != 060000)
			continue;
		if(sbuf.s_addr[0] == swap_dev_num)
			swap = open(p->dir_n, 0);
	}
	goto loop;
}

/*
 * Read data from process memory at given address.
 * Returns 0 on success, -1 on error.
 */
int readproc(struct proc *p_proc, uint addr, int len, char *result)
{
	int mf;
	int base_block;  /* base address in blocks */
	int blk_off;     /* block offset from addr */
	int byte_off;    /* byte offset within block */
	
	/* Determine which file to read from and calculate base block */
	if (p_proc->p_flag & SLOAD) {
		mf = mem;
		/* p_addr is in 4KB blocks, convert to 512-byte blocks */
		base_block = p_proc->p_addr * 8;
	} else {
		mf = swap;
		/* p_addr is already in 512-byte blocks */
		base_block = p_proc->p_addr;
	}
	
	/* Decompose addr into block offset and byte offset */
	blk_off = (addr >> 9) & 0177;  /* (addr / 512) & 0x7F */
	byte_off = addr & 0777;        /* addr % 512 (mask 0x1FF) */
	
	/* Seek to absolute block: base + offset */
	seek(mf, base_block + blk_off, 3);
	
	/* Seek to byte offset within that block */
	seek(mf, byte_off, 1);
	
	if (read(mf, result, len) != len)
		return(-1);
	
	return(0);
}

/*
 * Read a 2-byte word from process memory at given address.
 * Returns 0 on success, -1 on error.
 */
int readword(struct proc *p_proc, uint addr, int *result)
{
	return readproc(p_proc, addr, 2, (char *)result);
}

int prcom(int i)
{
	int sp, argc, argv;
	int argvi;
	char argbuf[17]; /* max 16 chars + null terminator */
	int j;

	/* Special handling for known processes */
	if (proc[i].p_pid == 0) {
		printf(" swapper");
		return(1);
	}
	if (proc[i].p_pid == 1) {
		printf(" init");
		return(1);
	}
	/* Check if swap device is available for swapped-out processes */
	if ((proc[i].p_flag & SLOAD) == 0 && swap < 0) {
		/* No swap device available */
		printf(" ???");
		return(0);
	}
	/* Read SP from offset 0xEFFE (top of user stack area) */
	if (readword(&proc[i], 0xEFFE, &sp) != 0)
		return(0);
	
	/* Read argc from [sp] */
	if (readword(&proc[i], sp, &argc) != 0)
		return(0);
	
	/* Read argv from [sp+2] */
	if (readword(&proc[i], sp+2, &argv) != 0)
		return(0);
	
	/* Print space before command */
	printf(" ");

	/* Iterate through arguments */
	for (j = 0; j < argc && j < 8; j++) { /* limit to 8 args */
		/* Read argv[j] pointer */
		if (readword(&proc[i], argv + j * 2, &argvi) != 0)
			break;		
		/* Read the string at argvi */
		if (readproc(&proc[i], argvi, 16, argbuf) != 0)
			break;
		argbuf[16] = 0; /* ensure null termination */
				
		/* Print argument */
		printf("%s", argbuf);
		if (j < argc - 1)
			printf(" ");
	}
	
	return(1);
}
