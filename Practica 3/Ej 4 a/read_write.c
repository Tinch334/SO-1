#include <stdlib.h>
#include <pthread.h>
#include <stdio.h>
#include <assert.h>

#include "read_write.h"

RW read_write_init() {
    RW rw = malloc(sizeof(_read_write));
    assert(rw != NULL);

    rw->readerCount = 0;
    rw->readRequest = false;
    pthread_mutex_t mut = PTHREAD_MUTEX_INITIALIZER;
    rw->mutex = mut;
    
    return rw;
}

void read_lock(RW rw) {
    pthread_mutex_lock(&rw->mutex);
    (rw->readerCount)++;
    pthread_mutex_unlock(&rw->mutex);

    while (rw->readRequest)
        pthread_cond_wait(&(rw->canRead), &(rw->mutex));
}

void read_unlock(RW rw) {
    if (rw->readerCount <= 0)
        pthread_cond_signal(&(rw->canWrite));
}

void write_lock(RW rw) {
    pthread_mutex_lock(&(rw->mutex));

    while (rw->readerCount > 0)
        pthread_cond_wait(&(rw->canWrite), &(rw->mutex));

    while(!rw->readRequest)
        pthread_cond_wait(&(rw->canWrite), &(rw->mutex));
}

void write_unlock(RW rw) {
    rw->readRequest = false;

    pthread_cond_signal(&(rw->canRead));
    pthread_cond_signal(&(rw->canWrite));
    pthread_mutex_unlock(&(rw->mutex));
}