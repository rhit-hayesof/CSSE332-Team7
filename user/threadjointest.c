#include "user.h"
#include "mythreads.h"

void thread_func(void *arg) {
  int *shared = (int *)arg;
  *shared = 100;
  thread_exit(0);
}

int main() {
  int shared_val = 42;
  void *retval;

  printf("Creating thread...\n");

  int tid = thread_create(thread_func, &shared_val);
  if (tid < 0) {
    printf("Failed to create thread\n");
    exit(1);
  }

  if (thread_join(tid, &retval) < 0) {
    printf("Failed to join thread\n");
    exit(1);
  }

  printf("Shared val = %d\n", shared_val);  // Should be 100
  exit(0);
}