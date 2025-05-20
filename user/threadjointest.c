#include "user.h"
#include "mythreads.h"

void thread_func(void *arg) {
  int *shared = (int *)arg;
  printf("Thread: updating shared value\n");
  *shared = 100;
  thread_exit(0);
}

int main() {
  int shared_val = 42;
  void *retval;

  printf("Main: creating thread...\n");

  int tid = thread_create(thread_func, &shared_val);
  if (tid < 0) {
    printf("Main: failed to create thread\n");
    exit(1);
  }

  if (thread_join(tid, &retval) < 0) {
    printf("Main: failed to join thread\n");
    exit(1);
  }

  printf("Main: shared_val = %d (expected 100)\n", shared_val);
  if (shared_val == 100) {
    printf("Main: test passed!\n");
  } else {
    printf("Main: test failed!\n");
  }

  exit(0);
}
