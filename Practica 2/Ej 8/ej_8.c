#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <fcntl.h>
#include <pthread.h>

int VISITORS = 0;
int N = 2000;

void * proc(void *arg) {
    int i;

    for (i = 0; i < N; i++) {
        int c;
        c = VISITORS;

        sleep(0.1);

        VISITORS = c + 1;
    }
    
    return NULL;
}



int main(){
    pthread_t thread, thread_2;
    int v = 1, w = 2;
    printf("Thread creation here\n");
    pthread_create(&thread, NULL, proc, &v);
    pthread_create(&thread_2, NULL, proc, &w);
    
    //Wait for thread to finish.
    pthread_join(thread, NULL);
    pthread_join(thread_2, NULL);

    printf("Main running, counted: %d \n", VISITORS);
}