#include "types.h"
#include "x86.h"
#include "defs.h"
#include "date.h"
#include "param.h"
#include "memlayout.h"
#include "mmu.h"
#include "proc.h"

extern struct {
  int key;
  char *frame;
  int refcount;
} shmtab[];

#define MAX_SHM 16
#define USERTOP 0xA0000  // 640KB typical user top in xv6

// extern struct shmregion shmtab[];
extern struct proc *proc;

extern int mappages(pde_t*, void*, uint, uint, int);

void* sys_shmget(void) {
  int key;
  if(argint(0, &key) < 0)
    return (void*)-1;

  for(int i = 0; i < MAX_SHM; i++){
    if(shmtab[i].key == key){
      shmtab[i].refcount++;
      struct proc *p = myproc();
      mappages(p->pgdir,
         (void*)(USERTOP - (i+1)*PGSIZE),
         PGSIZE,
         V2P(shmtab[i].frame),
         PTE_W | PTE_U);
      return (void*)(USERTOP - (i+1)*PGSIZE);
    }
  }

  for(int i = 0; i < MAX_SHM; i++){
    if(shmtab[i].key == 0){
      shmtab[i].key = key;
      shmtab[i].frame = kalloc();
      if (shmtab[i].frame == 0)
        return (void*)-1;
      shmtab[i].refcount = 1;
      memset(shmtab[i].frame, 0, PGSIZE);
      struct proc *p = myproc();
      mappages(p->pgdir, (char*)(USERTOP - (i+1)*PGSIZE), PGSIZE,
               V2P(shmtab[i].frame), PTE_W|PTE_U);
      return (void*)(USERTOP - (i+1)*PGSIZE);
    }
  }

  return (void*)-1;
}

int sys_shmrelease(void) {
  int key;
  if(argint(0, &key) < 0)
    return -1;

  for(int i = 0; i < MAX_SHM; i++){
    if(shmtab[i].key == key){
      shmtab[i].refcount--;
      if(shmtab[i].refcount == 0){
        kfree(shmtab[i].frame);
        shmtab[i].key = 0;
        shmtab[i].frame = 0;
      }
      return 0;
    }
  }
  return -1;
}

int
sys_fork(void)
{
  return fork();
}

int
sys_exit(void)
{
  exit();
  return 0;  // not reached
}

int
sys_wait(void)
{
  return wait();
}

int
sys_kill(void)
{
  int pid;

  if(argint(0, &pid) < 0)
    return -1;
  return kill(pid);
}

int
sys_getpid(void)
{
  return myproc()->pid;
}

int
sys_sbrk(void)
{
  int addr;
  int n;

  if(argint(0, &n) < 0)
    return -1;
  addr = myproc()->sz;
  if(growproc(n) < 0)
    return -1;
  return addr;
}

int
sys_sleep(void)
{
  int n;
  uint ticks0;

  if(argint(0, &n) < 0)
    return -1;
  acquire(&tickslock);
  ticks0 = ticks;
  while(ticks - ticks0 < n){
    if(myproc()->killed){
      release(&tickslock);
      return -1;
    }
    sleep(&ticks, &tickslock);
  }
  release(&tickslock);
  return 0;
}

// return how many clock tick interrupts have occurred
// since start.
int
sys_uptime(void)
{
  uint xticks;

  acquire(&tickslock);
  xticks = ticks;
  release(&tickslock);
  return xticks;
}