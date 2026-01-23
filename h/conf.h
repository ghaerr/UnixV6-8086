/*
 * Used to dissect integer device code
 * into major (driver designation) and
 * minor (driver parameter) parts.
 */
struct dev_t
{
    char    d_minor;
    char    d_major;
};

#define minor(x) ((x)&0xff) /* device minor */
#define major(x) ((x)>>8)   /* device major */
#define makedev(ma,mi) (((ma)<<8)+((mi)&0xff))

/*
 * Declaration of block device
 * switch. Each entry (row) is
 * the only link between the
 * main unix code and the driver.
 * The initialization of the
 * device switches is in the
 * file conf.c.
 */
struct bdevsw
{
    int (*d_open)(int d, int flag);
    int (*d_close)(int d, int flag);
    void (*d_strategy)(struct buf *bp);
    struct devtab *d_tab;
};
extern struct bdevsw bdevsw[];

/*
 * Nblkdev is the number of entries
 * (rows) in the block switch. It is
 * set in binit/bio.c by making
 * a pass over the switch.
 * Used in bounds checking on major
 * device numbers.
 */
extern int nblkdev;

/*
 * Character device switch
 */
struct cdevsw
{
    int (*d_open)(int d, int flag);
    int (*d_close)(int d, int flag);
    int (*d_read)(int d);
    int (*d_write)(int d);
    int (*d_sgtty)(int d, int *v);
};
extern struct cdevsw cdevsw[];

/*
 * Number of character switch entries.
 * Set by cinit/tty.c
 */
extern int nchrdev;
