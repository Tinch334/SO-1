#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#define _GNU_SOURCE
#include <unistd.h>
#include <sys/types.h>

pthread_barrier_t barrier;

void *threadFunc(void *arg) {
    pid_t p = gettid();
    printf("Before in: %d\n", p);
    
    pthread_barrier_wait(&barrier);

    printf("After in: %d\n", p);

    return NULL;
}

int main(int argc, char const *argv[])
{
    pthread_barrier_init(&barrier, NULL, 3);
    const int threadCount = 3;
    pthread_t threads[threadCount];

    for (size_t i = 0; i < threadCount; i++)
        pthread_create(&threads[i], NULL, threadFunc, NULL);

    for (size_t i = 0; i < threadCount; i++)
        pthread_join(threads[i], NULL);

    pthread_barrier_destroy(&barrier);

    return 0;
}
