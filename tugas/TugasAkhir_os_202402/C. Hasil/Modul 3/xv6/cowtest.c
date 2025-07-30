#include "types.h"
#include "stat.h"
#include "user.h"

int main() {
  char *p = sbrk(4096); // alokasikan mem
  p[0] = 'X';

  int pid = fork();
  if(pid == 0){
    p[0] = 'Y';
    printf(1, "Child sees: %c\n", p[0]);
    exit();
  } else {
    wait();
    printf(1, "Parent sees: %c\n", p[0]);
  }
  exit();
}