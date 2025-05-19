#include "types.h"
#include "riscv.h"
#include "defs.h"
#include "param.h"
#include "memlayout.h"
#include "spinlock.h"
#include "proc.h"

uint64
sys_exit(void)
{
  int n;
  argint(0, &n);
  exit(n);
  return 0;  // not reached
}

uint64
sys_getpid(void)
{
  return myproc()->pid;
}

uint64
sys_fork(void)
{
  return fork();
}

uint64
sys_wait(void)
{
  uint64 p;
  argaddr(0, &p);
  return wait(p);
}

uint64
sys_sbrk(void)
{
  uint64 addr;
  int n;

  argint(0, &n);
  addr = myproc()->sz;
  if(growproc(n) < 0)
    return -1;
  return addr;
}

uint64
sys_sleep(void)
{
  int n;
  uint ticks0;

  argint(0, &n);
  acquire(&tickslock);
  ticks0 = ticks;
  while(ticks - ticks0 < n){
    if(killed(myproc())){
      release(&tickslock);
      return -1;
    }
    sleep(&ticks, &tickslock);
  }
  release(&tickslock);
  return 0;
}

uint64
sys_kill(void)
{
  int pid;

  argint(0, &pid);
  return kill(pid);
}

// return how many clock tick interrupts have occurred
// since start.
uint64
sys_uptime(void)
{
  uint xticks;
  acquire(&tickslock);
  xticks = ticks;
  release(&tickslock);
  return xticks;
}

uint64 sys_spoon(void) {
  uint64 addr;
  argaddr(0, &addr);
  return spoon((void*)addr);
}

uint64 
sys_thread_create(void) {
  uint64 fcn_addr, arg_addr;

  argaddr(0, &fcn_addr);
  argaddr(1, &arg_addr);

  return thread_create((void (*)(void *))fcn_addr, (void *)arg_addr);
}

uint64 sys_thread_join(void) {
  int tid;
  uint64 retval_ptr;

  if (argint(0, &tid) < 0 || argaddr(1, &retval_ptr) < 0)
    return -1;

  void *retval;
  int rc = thread_join(tid, &retval);
  
  if (rc >= 0 && retval_ptr != 0) {
    if (copyout(myproc()->pagetable, retval_ptr, (char *)&retval, sizeof(void *)) < 0)
      return -1;
  }

  return rc;
}

uint64
sys_thread_exit(void)
{
  uint64 retval;
  if (argaddr(0, &retval) < 0)
    return -1;

  thread_exit((void *)retval);
  return 0; // never reached
}

