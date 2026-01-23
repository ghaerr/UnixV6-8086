#include "unix.h"

int main()
{
    fprintf(2, "uid %d euid %d gid %d rgid %d\n", 
        getuid() >> 8, getuid() & 0xff,
        getgid() >> 8, getgid() & 0xff);

    exit();
}
