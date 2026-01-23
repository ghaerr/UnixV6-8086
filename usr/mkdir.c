#include "unix.h"

int main(int argc, char **argv)
{
    signal(SIGHUP, SIG_IGN);
    signal(SIGINT, SIG_IGN);
    signal(SIGQIT, SIG_IGN);
    signal(SIGPIPE, SIG_IGN);

    if(argc < 2) {
        fprintf(stderr, "mkdir: arg count\n");
        exit1(1);
    }
    while(--argc)
        mkdir(*++argv);
        
    return 0;
}
