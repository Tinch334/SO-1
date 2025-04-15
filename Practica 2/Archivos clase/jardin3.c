#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <fcntl.h>
#include <pthread.h>

int VISITORS = 0;
pthread_mutex_t mutex_visitantes = PTHREAD_MUTEX_INITIALIZER;
/*
    The critical section needs to be as small as possible for efficiency.
*/

void *contar_thread(void* v){
    for(int i = 0; i < *((int *)v); i++) {
        //Lock
        pthread_mutex_lock(&mutex_visitantes);

        //CRITICAL SECTION: Not atomic, requires three assembly instructions
        VISITORS++;
        //printf("Counted 1: %d\n", VISITORS);

        //Unlock
        pthread_mutex_unlock(&mutex_visitantes);
    }

    pthread_exit(NULL);
}

int main(){
    pthread_t thread, thread_2, thread_3;
    int v = 200000, w = 200000, x = 100000;
    printf("Thread creation here\n");
    pthread_create(&thread, NULL, contar_thread, &v);
    pthread_create(&thread_2, NULL, contar_thread, &w);
    pthread_create(&thread_3, NULL, contar_thread, &x);
    
    //Wait for thread to finish.
    pthread_join(thread, NULL);
    pthread_join(thread_2, NULL);
    pthread_join(thread_3, NULL);

    pthread_mutex_destroy(&mutex_visitantes);

    printf("Main running, counted: %d \n", VISITORS);
}