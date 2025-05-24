#include "mythreads.h"
#include "user.h"

int *p = 0;
volatile int ready = 0;         // thread2 waits until this is set
volatile int memory_freed = 0;  // thread2 checks this before accessing p

void thread1(void *arg) {
  p = (int*)sbrk(4096); // allocate 1 page
  p[0] = 3;
  p[1] = 2;

  __sync_synchronize(); // memory barrier
  ready = 1;

  sleep(20); // give thread2 time to read

  sbrk(-4096); // deallocate

  memory_freed = 1;
  thread_exit(0);
}

void thread2(void *arg) {
  while (!ready) sleep(1); // wait until data is ready

  __sync_synchronize(); // memory barrier

  printf("Thread2: reading values...\n");
  if (!memory_freed && p && p[0] == 3 && p[1] == 2) {
    printf("Success: shared mapping propagated\n");
  } else if (memory_freed) {
    printf("Skipped access: memory has been unmapped\n");
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