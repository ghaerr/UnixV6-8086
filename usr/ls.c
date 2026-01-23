#include "unix.h"

char *fmtname(char *path)
{
    static char buf[DIRSIZ + 1];
    char *p;

    /* Find first character after last slash. */
    for (p = path + strlen(path); p >= path && *p != '/'; p--)
        ;
    p++;

    /* Return blank-padded name. */
    if (strlen(p) >= DIRSIZ)
        return p;
    memmove(buf, p, strlen(p));
    memset(buf + strlen(p), ' ', DIRSIZ - strlen(p));
    return buf;
}

#define S_TYPE 0060000  /* file type :		*/
#define S_PLAIN 0000000 /* plain file		*/
#define S_DIREC 0040000 /* directory		*/
#define S_CHAR 0020000  /* char special		*/
#define S_BLOCK 0060000 /* block special	*/

#define minor(x) ((x)&0xff) /* minor device szam		*/
#define major(x) ((x) >> 8) /* major device szam		*/

uint year;

int stat_type(struct stat *p)
{
    switch (p->s_flags & S_TYPE)
    {
    case S_PLAIN:
        return T_FILE;
    case S_DIREC:
        return T_DIR;
    default:
        return T_DEV;
    }
}

void lsfile(char *path)
{
    struct stat st;
    char *cp;
    char buf[80];
    int len;

    if (stat(path, &st) < 0)
    {
        printf("ls: cannot stat %s\n", path);
        return;
    }
    switch (st.s_flags & S_TYPE)
    {
    case S_PLAIN:
        buf[0] = '-';
        break;
    case S_DIREC:
        buf[0] = 'd';
        break;
    case S_CHAR:
        buf[0] = 'c';
        break;
    case S_BLOCK:
        buf[0] = 'b';
        break;
    }
    len = 1;
    len += sprintf(&buf[len], "%c%c%c", (st.s_flags & 0400) ? 'r' : '-',
           (st.s_flags & 0200) ? 'w' : '-',
           (st.s_flags & 0100) ? ((st.s_flags & 04000) ? 's' : 'x') : '-');
    len += sprintf(&buf[len], "%c%c%c", (st.s_flags & 0040) ? 'r' : '-',
           (st.s_flags & 0020) ? 'w' : '-',
           (st.s_flags & 0010) ? ((st.s_flags & 02000) ? 's' : 'x') : '-');
    len += sprintf(&buf[len], "%c%c%c ", (st.s_flags & 0004) ? 'r' : '-',
           (st.s_flags & 0002) ? 'w' : '-',
           (st.s_flags & 0001) ? 'x' : '-');
    if ((st.s_flags & S_TYPE) != S_CHAR && (st.s_flags & S_TYPE) != S_BLOCK)
    {
        len += sprintf(&buf[len], "%3u %u/%u ", st.s_nlinks, st.s_gid, st.s_uid);
        len += sprintf(&buf[len], "%7u ", st.s_size1);
    }
    else
    {
        len += sprintf(&buf[len], "%3u %u/%u         ", st.s_nlinks, 
            major(st.s_addr[0]), minor(st.s_addr[0]));
    }
    cp = ctime(st.s_modtime);
    if (st.s_modtime[0] < year)
        len += sprintf(&buf[len], " %-7.7s %-4.4s", cp + 4, cp + 20);
    else
        len += sprintf(&buf[len], " %-12.12s", cp + 4);
    len += sprintf(&buf[len], " %s\n", fmtname(path));
    write(1, buf, len);
}

void ls(char *path)
{
    char buf[512], *p;
    int fd;
    struct dirent de;
    struct stat st;

    if ((fd = open(path, 0)) < 0)
    {
        fprintf(2, "ls: cannot open %s\n", path);
        return;
    }

    time(st.s_modtime);
    year = st.s_modtime[0] - 245; /* 6 months ago */

    if (fstat(fd, &st) < 0)
    {
        fprintf(2, "ls: cannot stat %s\n", path);
        close(fd);
        return;
    }

    switch (stat_type(&st))
    {
    case T_FILE:
        lsfile(path);
        break;

    case T_DIR:
        if (strlen(path) + 1 + DIRSIZ + 1 > sizeof buf)
        {
            printf("ls: path too long\n");
            break;
        }
        strcpy(buf, path);
        p = buf + strlen(buf);
        *p++ = '/';
        while (read(fd, &de, sizeof(de)) == sizeof(de))
        {
            if (de.inum == 0)
                continue;
            memmove(p, de.name, DIRSIZ);
            p[DIRSIZ] = 0;
            if (stat(buf, &st) < 0)
            {
                printf("ls: cannot stat %s\n", buf);
                continue;
            }
            lsfile(buf);
        }
        break;
    }
    close(fd);
}

int main(int argc, char *argv[])
{
    int i;

    if (argc < 2)
    {
        ls(".");
        exit();
    }
    for (i = 1; i < argc; i++)
        ls(argv[i]);

    return 0;
}
