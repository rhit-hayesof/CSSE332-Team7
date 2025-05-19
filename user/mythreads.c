#include "user.h"
#include "threads.h"

int thread_create(void (*fcn)(void *), void *arg) {
  return syscall(SYS_thread_create, fcn, arg);
}

int thread_join(int tid, void **retval) {
  return syscall(SYS_thread_join, tid, retval);
}

void thread_exit(void *retval) {
  syscall(SYS_thread_exit, retval);
}