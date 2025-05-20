#include "user.h"
#include "mythreads.h"

// Thread function that modifies a shared integer
void thread_func(void *arg) {
  int *shared = (int *)arg;
  *shared = 100; // change the value to verify memory sharing
  thread_exit(0); // exit cleanly
}

int main() {
  int shared_val = 42; // initial value before thread runs
  void *retval;

  printf("Main: creating thread...\n");

  // Create a thread that will modify shared_val
  int tid = thread_create(thread_func, &shared_val);
  if (tid < 0) {
    printf("Main: failed to create thread\n");
    exit(1);
  }

  // Wait for the thread to finish
  if (thread_join(tid, &retval) < 0) {
    printf("Main: failed to join thread\n");
    exit(1);
  }

  // Check that the thread’s changes to shared_val are visible
  printf("Main: shared_val = %d (expected 100)\n", shared_val);
  if (shared_val == 100) {
    printf("Main: test passed!\n");
  } else {
    printf("Main: test failed!\n");
  }

  exit(0);
}
