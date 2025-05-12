#include "user.h"
#include "mythreads.h"

void thread_func(void *arg) {
  int id = *(int *)arg;
  printf("Hello from thread! id = %d\n", id);
  exit(0);  // This thread exits here (no join yet)
}

int main() {
  int arg = 42;

  printf("Creating thread...\n");
  int tid = thread_create(thread_func, &arg);

  if (tid < 0) {
    printf("Thread creation failed!\n");
    exit(1);
  }

  printf("Thread created with tid: %d\n", tid);
  exit(0);
}
