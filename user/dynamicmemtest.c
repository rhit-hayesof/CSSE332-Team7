#import "mythreads.h"
#include "user.h"

int *p;

void thread1(void *arg) {
  p = (int*)sbrk(4096);

  p[0] = 3;
  p[1] = 2;
}

void thread2(void *arg) {
  sleep(50);

  if (*p == 0xdeadbeef) {
    printf("Failure: page not mapped\n");
    exit(1);
  }

  if (p[0] == 3 && p[1] == 2) {
    printf("Success\n");
  }
  else {
    printf("Failure: values in page not set\n");
  }
}

int main() {
  *p = 0xdeadbeef;

  printf("Creating threads\n");
  int t1 = thread_create(thread1, 0);
  int t2 = thread_create(thread2, 0);

  printf("Joining threads\n");
  thread_join(t1, 0);
  thread_join(t2, 0);
}
