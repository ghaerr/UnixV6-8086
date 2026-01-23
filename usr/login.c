#include "unix.h"

char *argv[] = { "sh", 0 };

int
main(int argc, char **argv)
{
    if(argc < 2){
        fprintf(2, "usage: login user|root\n");
        exit();
    }
    if(strcmp(argv[1], "root") == 0){
        setgid(0);
        setuid(0);
    } else if(strcmp(argv[1], "user") == 0){
        setgid(3);
        setuid(3);
    } else {
        fprintf(2, "usage: login user|root\n");
        exit();
    }

    exec("/bin/sh", argv);
    exit();
}
