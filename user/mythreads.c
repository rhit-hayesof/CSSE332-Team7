// user/mythreads.c
#include "user.h"
#include "mythreads.h"
#include "syscall.h"

int thread_create(void (*start_routine)(void *), void *arg) {
  return syscall(SYS_thread_create, start_routine, arg);
}