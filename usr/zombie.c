#include "unix.h"

int main(void)
{
  if(fork() > 0)
    sleep(5);  // Let child exit before parent.
  exit();
}
