#include "unix.h"

int
main(int argc, char *argv[])
{
  int i;

  if(argc < 2){
    printf("Usage: rm files...\n");
    exit();
  }

  for(i = 1; i < argc; i++){
    if(unlink(argv[i]) < 0){
      printf("rm: %s failed to delete\n", argv[i]);
      break;
    }
  }

  exit();
}
