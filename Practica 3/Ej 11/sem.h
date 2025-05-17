#ifndef __SEM_H__
#define __SEM_H__

#include <stdlib.h>
#include <pthread.h>
#include <assert.h>

typedef struct
{
    size_t counter;
    pthread_cond_t signal;
    pthread_mutex_t mutex;
} sem;

void sem_init(sem* s, const size_t initial_value);
void sem_post(sem* s);
void sem_wait(sem* s);

#endif //__SEM_H__