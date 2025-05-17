#ifndef __BARRIER_H__
#define __BARRIER_H__

#include <pthread.h>
#include <semaphore.h>

typedef struct {
    int targetN;
    int n;

    pthread_mutex_t mutex;
    pthread_cond_t wait_signal;
} barrier;


void barrier_init(barrier *b, int n);

void barrier_wait(barrier *b);

#endif //__BARRIER_H__