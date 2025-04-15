#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <fcntl.h>
#include <pthread.h>

const int ARR[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
const int LEN = 10;
int PTR = 0;
int SUM = 0;

pthread_mutex_t mutex_visitantes = PTHREAD_MUTEX_INITIALIZER;

void *sum(void *arg) {
    while (PTR < LEN) {
        pthread_mutex_lock(&mutex_visitantes);

        SUM += ARR[PTR];
        PTR++;

        pthread_mutex_unlock(&mutex_visitantes);
    }
    
    return NULL;
}



int main(){
    pthread_t thread, thread_2;
    printf("Thread creation here\n");
    pthread_create(&thread, NULL, sum, NULL);
    pthread_create(&thread_2, NULL, sum, NULL);
    
    //Wait for thread to finish.
    pthread_join(thread, NULL);
    pthread_join(thread_2, NULL);

    int sum2 = 0;

    for (int i = 0; i < LEN; ++i)
     sum2 += ARR[i];

    printf("Main running, sum thread: %d, sum no thread: %d \n", SUM, sum2);
}