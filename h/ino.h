/*
 * Inode structure as it appears on
 * the disk. Not used by the system,
 * but by things like check, df, dump.
 */
struct inode
{
    int     i_mode;         /* see below */
    char    i_nlink;        /* number of links to inode */
    char    i_uid;          /* owner uid */
    char    i_gid;          /* owner gid */
    char    i_size0;        /* bits 23..16 of length */
    int     i_size1;        /* bits 15..0 of length */
    int     i_addr[8];      /* blocks of the file */
    int     i_atime[2];     /* time of last access */
    int     i_mtime[2];     /* time of last modification */
};

/* modes */
#define IALLOC  0100000     /* allocated */
#define IFMT     060000     /* mask for type */
#define  IFDIR   040000     /* inode is directory */
#define  IFCHR   020000     /* inode is char special */
#define  IFBLK   060000     /* inode is block special */
#define ILARG    010000     /* file is indirect */
#define ISUID     04000     /* set owner id on exec */
#define ISGID     02000     /* set group id on exec */
#define ISVTX     01000     /* keep text on swap */
#define IREAD      0400     /* can be read */
#define IWRITE     0200     /* can be written */
#define IEXEC      0100     /* can be executed */
