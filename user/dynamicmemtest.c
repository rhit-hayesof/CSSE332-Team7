#include "mythreads.h"
#include "user.h"

int *p = 0;
volatile int ready = 0;         // thread2 waits until this is set
volatile int memory_freed = 0;  // thread2 checks this before accessing p

void thread1(void *arg) {
  // Allocate 1 page and initialize data
  p = (int*)sbrk(4096);
  p[0] = 3;
  p[1] = 2;

  // Signal that memory and data are ready
  __sync_synchronize();
  ready = 1;

  // Sleep to allow thread2 time to safely read
  sleep(10);

  // Signal that memory is about to be freed
  memory_freed = 1;
  __sync_synchronize();

  // Now safely deallocate
  sbrk(-4096);

  thread_exit(0);
}

void thread2(void *arg) {
  // Wait until data is ready
  while (!ready) sleep(1);

  __sync_synchronize();

  printf("Thread2: reading values...\n");

  // Only access memory if it has not been freed
  if (memory_freed) {
    printf("Skipped access: memory has been unmapped\n");
  } else if (p && p[0] == 3 && p[1] == 2) {
    printf("Success: shared mapping propagated\n");
  } else {
    printf("Failure: unexpected data\n");
  }

  thread_exit(0);
}

int main() {
  printf("Creating threads...\n");

  int t1 = thread_create(thread1, 0);
  int t2 = thread_create(thread2, 0);

  if (t1 < 0 || t2 < 0) {
    printf("Thread creation failed\n");
    exit(1);
  }

  printf("Joining threads\n");
  thread_join(t1, 0);
  thread_join(t2, 0);

  printf("Test complete.\n");
  exit(0);
}
