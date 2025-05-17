#include "sem.h"

void sem_init(sem* s, const size_t initial_value) {
    s->counter = initial_value;
    assert(!pthread_cond_init(&s->signal, NULL));
    assert(!pthread_mutex_init(&s->mutex, NULL));
}

void sem_post(sem* s) {
    pthread_mutex_lock(&s->mutex);
    s->counter++;
    pthread_mutex_unlock(&s->mutex);
    pthread_cond_signal(&s->signal);
}

void sem_wait(sem* s) {
    pthread_mutex_lock(&s->mutex);
    s->counter--;

    if (s->counter < 0)
        pthread_cond_wait(&s->signal, &s->mutex);

    //When "pthread_cond_wait" unclocks the mutex will be set, release it.
    pthread_mutex_unlock(&s->mutex);
}