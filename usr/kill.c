#include "unix.h"

int
main(int argc, char **argv)
{
  int i, pid, sig;

  if(argc < 2){
    fprintf(2, "usage: kill pid...\n");
    exit();
  }
  pid = atoi(argv[1]);
  sig = argc > 2 ? atoi(argv[2]) : SIGHUP;
  kill(pid, sig);
  exit();
}
