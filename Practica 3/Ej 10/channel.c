#include "channel.h"

void channel_init(channel *c) {
    c->arr_size = INITIAL_SIZE;
    c->arr_ptr = 0;
    c->arr = malloc(c->arr_size * sizeof(int));
    assert(c->arr != NULL);

    sem_init(&c->channel_counter, 0, 0);
    pthread_mutex_init(&(c->mutex), NULL);
}

void chan_write(channel *c, int v) {
    pthread_mutex_lock(&(c->mutex));

    c->arr_ptr++;
    //If the array is full allocate more space.
    if (c->arr_ptr >= c->arr_size) {
        c->arr_size *= 2;

        c->arr = (int *)realloc(c->arr, c->arr_size * sizeof(int));
        assert(c->arr != NULL);
    }

    c->arr[c->arr_ptr] = v;

    sem_post(&(c->channel_counter));
    pthread_mutex_unlock(&(c->mutex));
}


int chan_read(channel *c) {
    int ret_val;

    sem_wait(&(c->channel_counter));
    pthread_mutex_lock(&(c->mutex));

    //No need to check indexes, the semaphore handles that.
    ret_val = c->arr[c->arr_ptr];
    c->arr_ptr--;

    pthread_mutex_unlock(&(c->mutex));

    return ret_val;
}