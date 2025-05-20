#include "user.h"
#include "mythreads.h"

// A shared structure to test memory sharing between threads
struct shared_data {
  int x;
  int y;
};

// Thread 1: increments x by 10
void worker1(void *arg) {
  struct shared_data *data = (struct shared_data *)arg;
  data->x += 10;
  sleep(20); // pause to allow overlap with worker2
  thread_exit(0);
}

// Thread 2: increments y by 20
void worker2(void *arg) {
  struct shared_data *data = (struct shared_data *)arg;
  data->y += 20;
  sleep(10); // staggered to simulate non-deterministic scheduling
  thread_exit(0);
}

int main() {
  struct shared_data data;
  data.x = 1;
  data.y = 2;
  void *retval;

  printf("Main: creating threads...\n");

  // Create two threads, both operating on the same shared struct
  int tid1 = thread_create(worker1, &data);
  int tid2 = thread_create(worker2, &data);
  if (tid1 < 0 || tid2 < 0) {
    printf("Main: failed to create threads\n");
    exit(1);
  }

  // Wait for both threads to finish
  if (thread_join(tid1, &retval) < 0 || thread_join(tid2, &retval) < 0) {
    printf("Main: failed to join threads\n");
    exit(1);
  }

  // Validate that both threads' changes to the shared struct are visible
  printf("Main: data.x = %d (expected 11), data.y = %d (expected 22)\n", data.x, data.y);
  if (data.x == 11 && data.y == 22) {
    printf("Main: memory sharing test passed!\n");
  } else {
    printf("Main: memory sharing test failed!\n");
  }

  exit(0);
}
