/* init: The initial user-level program */

#include "unix.h"

char *argv[] = { "sh", 0 };

void catch(void)
{
  signal(SIGHUP, catch);
  signal(SIGINT, catch);
  signal(SIGQIT, catch);
  signal(SIGPIPE, catch);
}

int
main(void)
{
  int pid, wpid;

  unlink("/etc/mtab");
  if(open("/dev/console", 2) < 0){
    mknod("/dev/console", 0120666, 0);
    open("/dev/console", 2);
  }
  dup(0);  /* stdout */
  dup(0);  /* stderr */

  catch();

  for(;;){
    printf("init: starting sh\n");
    pid = fork();
    if(pid < 0){
      printf("init: fork failed\n");
      exit();
    }
    if(pid == 0){
      exec("/bin/sh", argv);
      printf("init: exec sh failed\n");
      exit();
    }
    while((wpid=wait()) >= 0 && wpid != pid)
      printf("zombie!\n");
  }
}
