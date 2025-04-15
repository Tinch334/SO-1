#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <fcntl.h>
#include <pthread.h>

int VAR1 = 0;
int VAR2 = 0;

pthread_mutex_t mutex_1 = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex_2 = PTHREAD_MUTEX_INITIALIZER;



void *fun1 (void *v) {
    while (1) {
        pthread_mutex_lock(&mutex_1);
        
        VAR1++;

        pthread_mutex_lock(&mutex_2);

        VAR1 += VAR2;

        printf("In 1 - VAR1: %d - VAR2: %d\n", VAR1, VAR2);

        pthread_mutex_unlock(&mutex_2);
        pthread_mutex_unlock(&mutex_1);
    }

    pthread_exit(NULL);
}

void *fun2 (void *v) {
    while (1) {
        pthread_mutex_lock(&mutex_2);
        
        VAR2++;

        pthread_mutex_lock(&mutex_1);

        VAR2 += VAR1;

        printf("In 2 - VAR1: %d - VAR2: %d\n", VAR1, VAR2);

        pthread_mutex_unlock(&mutex_1);
        pthread_mutex_unlock(&mutex_2);
    }

    pthread_exit(NULL);
}

int main(){
    pthread_t thread, thread_2;
    printf("Thread creation here\n");
    pthread_create(&thread, NULL, fun1, NULL);
    pthread_create(&thread_2, NULL, fun2, NULL);
    
    //Wait for thread to finish.
    pthread_join(thread, NULL);
    pthread_join(thread_2, NULL);

    pthread_mutex_destroy(&mutex_1);
    pthread_mutex_destroy(&mutex_2);

    printf("Main running, V1: %d - V2: %d\n", VAR1, VAR2);
}