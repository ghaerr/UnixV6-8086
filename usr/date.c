#include "unix.h"

int	timbuf[2];

int main()
{
    time(timbuf);
    printf(ctime(timbuf));

    return 0;
}
