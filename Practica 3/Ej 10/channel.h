#ifndef __CHANNEL_H__
#define __CHANNEL_H__

#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include <assert.h>

#define INITIAL_SIZE 10

typedef struct {
    size_t arr_size;
    size_t arr_ptr;
    int *arr;

    sem_t channel_counter;
    pthread_mutex_t mutex;
} channel;

void channel_init(channel *c);
void chan_write(channel *c, int v);
int chan_read(channel *c);

#endif //__CHANNEL_H__