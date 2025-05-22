#include "mythreads.h"
#include "user.h"

int *p = 0;

void thread1(void *arg) {
  // Allocate memory (should be propagated to other threads)
  p = (int*)sbrk(4096); //1 page worth of memory

  p[0] = 3;
  p[1] = 2;

  thread_exit(0);
}

void thread2(void *arg) {
  //Wait for thread1 to finish
  while (p == 0) {
    sleep(10); 
  }

  if (p[0] == 3 && p[1] == 2) {
    printf("Success: shared mapping propagated\n");
  }
  else {
    printf("Failure: values not propagated\n");
  }

  thread_exit(0);
}

int main() {
  printf("Creating threads...\n");

  int t1 = thread_create(thread1, 0);
  int t2 = thread_create(thread2, 0);

  if(t1 < 0 || t2 < 0) {
    printf("Thread creation failed\n");
    exit(1);
  }

  printf("Joining threads\n");
  thread_join(t1, 0);
  thread_join(t2, 0);

  printf("Test complete.\n");
  exit(0);
}
