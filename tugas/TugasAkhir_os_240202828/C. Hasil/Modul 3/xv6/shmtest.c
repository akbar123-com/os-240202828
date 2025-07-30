#include "types.h"
#include "stat.h"
#include "user.h"

int main() {
  char *shm = (char*) shmget(42);
  shm[0] = 'A';

  if(fork() == 0){
    char *shm2 = (char*) shmget(42);
    printf(1, "Child reads: %c\n", shm2[0]);
    shm2[1] = 'B';
    shmrelease(42);
    exit();
  } else {
    wait();
    printf(1, "Parent reads: %c\n", shm[1]);
    shmrelease(42);
  }
  exit();
}