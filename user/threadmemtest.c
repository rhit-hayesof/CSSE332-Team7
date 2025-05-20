#include "user.h"
#include "mythreads.h"

struct shared_data {
  int x;
  int y;
};

void worker1(void *arg) {
  struct shared_data *data = (struct shared_data *)arg;
  data->x += 10;
  sleep(20); // ensure worker2 can run
  thread_exit(0);
}

void worker2(void *arg) {
  struct shared_data *data = (struct shared_data *)arg;
  data->y += 20;
  sleep(10); // stagger threads slightly
  thread_exit(0);
}

int main() {
  struct shared_data data;
  data.x = 1;
  data.y = 2;
  void *retval;

  printf("Main: creating threads...\n");

  int tid1 = thread_create(worker1, &data);
  int tid2 = thread_create(worker2, &data);
  if (tid1 < 0 || tid2 < 0) {
    printf("Main: failed to create threads\n");
    exit(1);
  }

  if (thread_join(tid1, &retval) < 0 || thread_join(tid2, &retval) < 0) {
    printf("Main: failed to join threads\n");
    exit(1);
  }

  printf("Main: data.x = %d (expected 11), data.y = %d (expected 22)\n", data.x, data.y);
  if (data.x == 11 && data.y == 22) {
    printf("Main: memory sharing test passed!\n");
  } else {
    printf("Main: memory sharing test failed!\n");
  }

  exit(0);
}
