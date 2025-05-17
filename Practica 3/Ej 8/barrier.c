#include "barrier.h"
#include <stdlib.h>

void barrier_init(barrier *b, int n) {
    b->targetN = n;
    b->n = 0;
    pthread_mutex_init(&(b->mutex), NULL);
    pthread_cond_init(&(b->wait_signal), NULL);
}

void barrier_wait(barrier *b) {
    pthread_mutex_lock(&(b->mutex));

    (b->n)++;

    if (b->n == b->targetN) {
        pthread_cond_broadcast(&(b->wait_signal));
        b->n = 0;
        pthread_mutex_unlock(&(b->mutex));
    }
    else {
        pthread_cond_wait(&(b->wait_signal), &(b->mutex));
        pthread_mutex_unlock(&(b->mutex));
    }
}