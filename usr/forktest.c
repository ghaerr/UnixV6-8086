/*
Test that fork fails gracefully.
Tiny executable so that the limit can be filling the proc table.
*/
#include "unix.h"

int N = 100;
int N_Again = 0;
int N_Wait = 0;

void forktest(void)
{
  int n, pid;

  printf("fork test\n");  

  for(n=0; n<N; n++){
again:
    pid = fork();
    if(pid < 0){
      if(pid == EAGAIN){
        if(N_Again == 0) N_Again = n + 1;
        if(wait()>0) N_Wait++;
        goto again;
      }
      printf("fork failed, error %d\n", pid);
      break;
    }
    if(pid == 0){
      printf("child pid = %d exit\n", getpid());
      exit();
    }
    printf("create child pid = %d\n", pid);
  }

  if(n == N){
    printf("fork claimed to work %d times!\n", n);
  }
  
  printf("N_Wait = %d\n", N_Wait);
  for(; N_Wait < N; N_Wait++){
    if(wait() < 0){
      printf("wait stopped early, N_Wait = %d\n", N_Wait);
      exit();
    }
  }

  if((n = wait()) != -1){
    printf("wait got too many, last wait returns %d\n", n);
    exit();
  }

  printf("fork test OK, N_Again = %d\n", N_Again);
}

int main(int argc, char **argv)
{
  if(argc==2) N = atoi(argv[1]);
  forktest();
  exit();
}
