// mythreads.h
#ifndef MYTHREADS_H
#define MYTHREADS_H

typedef int thread_t;

int thread_create(void (*start_routine)(void *), void *arg);
int thread_join(thread_t thread);

#endif
