#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <pthread.h>
#include <stdbool.h>

#define VISITOR_NUM 100000

#define N 4

volatile int NUM[N];
volatile bool CHOOSING[N];

//Global visitor counter.
volatile int VISITORS = 0;


void lock(const int index) {
    //We assume that assignment is an atomic operation.
    CHOOSING[index] = true;

    //Critical section 1.
    int max = NUM[0];

    for (int i = 1; i < N; i++)
        if (NUM[i] > max)
            max = NUM[i];

    NUM[index] = max + 1;

    CHOOSING[index] = false;

    for (int j = 0; j < N; j++)
    {
        //Busy waiting.
        while (CHOOSING[j]);

        //Busy waiting.
        while ((NUM[j] != 0) && ((NUM[j] < NUM[index]) || ((NUM[j] == NUM[index]) && (j < index))));
    }
}

void unlock(const int index) {
    NUM[index] = 0;
}

void *contar_thread(void* v){
    int id = *((int *)v);

    for(int i = 0; i < VISITOR_NUM; i++) {
        //Lock
        lock(id);

        //CRITICAL SECTION: Not atomic, requires three assembly instructions
        VISITORS++;
        printf("Counted %d: %d\n", id, VISITORS);

        //Unlock
        unlock(id);
    }

    pthread_exit(NULL);
}

int main(){
    pthread_t t1, t2, t3, t4;
    int u = 0, v = 1, w = 2, x = 3;

    for (int i = 0; i < N; i++) {
        CHOOSING[i] = false;
        NUM[i] = 0;
    }

    printf("Thread creation here\n");
    pthread_create(&t1, NULL, contar_thread, &u);
    pthread_create(&t2, NULL, contar_thread, &v);
    pthread_create(&t3, NULL, contar_thread, &w);
    pthread_create(&t4, NULL, contar_thread, &x);
    
    //Wait for thread to finish.
    pthread_join(t1, NULL);
    pthread_join(t2, NULL);
    pthread_join(t3, NULL);
    pthread_join(t4, NULL);

    printf("Main running, counted: %d \n", VISITORS);
}