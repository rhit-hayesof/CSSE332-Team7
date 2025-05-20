#include "user.h"
#include "mythreads.h"

#define NTHREADS 10

int shared_array[NTHREADS]; // Shared memory array each thread will update

// Each thread sets shared_array[thread_index] = thread_index * 10
void thread_work(void *arg) {
  int index = *(int *)arg;
  shared_array[index] = index * 10;
  sleep(5); // add slight delay to simulate interleaving
  thread_exit(0);
}

int main() {
  int tids[NTHREADS];
  int args[NTHREADS]; // One arg per thread to pass index safely
  void *retval;

  printf("Main: starting stress test with %d threads\n", NTHREADS);

  // Initialize shared_array to invalid values
  for (int i = 0; i < NTHREADS; i++) {
    shared_array[i] = -1;
  }

  // Create threads
  for (int i = 0; i < NTHREADS; i++) {
    args[i] = i; // unique index for each thread
    tids[i] = thread_create(thread_work, &args[i]);
    if (tids[i] < 0) {
      printf("Main: failed to create thread %d\n", i);
      exit(1);
    }
  }

  // Join all threads
  for (int i = 0; i < NTHREADS; i++) {
    if (thread_join(tids[i], &retval) < 0) {
      printf("Main: failed to join thread %d\n", tids[i]);
      exit(1);
    }
  }

  // Verify shared_array was updated correctly
  int success = 1;
  for (int i = 0; i < NTHREADS; i++) {
    if (shared_array[i] != i * 10) {
      printf("Main: shared_array[%d] = %d (expected %d)\n", i, shared_array[i], i * 10);
      success = 0;
    }
  }

  if (success) {
    printf("Main: stress test passed! All threads completed successfully.\n");
  } else {
    printf("Main: stress test failed. Shared memory was corrupted.\n");
  }

  exit(0);
}
