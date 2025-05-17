#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <pthread.h>
#include <stdbool.h>
#include <omp.h>

#define VISITOR_NUM 10000

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

void contar_thread(){
    int id = omp_get_thread_num();

    for(int i = 0; i < VISITOR_NUM; i++) {
        //Lock
        lock(id);

        //CRITICAL SECTION: Not atomic, requires three assembly instructions
        VISITORS++;
        printf("Counted %d: %d\n", id, VISITORS);

        //Unlock
        unlock(id);
    }

    return;
}

int main(){
    for (int i = 0; i < N; i++) {
        CHOOSING[i] = false;
        NUM[i] = 0;
    }

    #pragma omp parallel
    {
        contar_thread();
    }  

    printf("Main running, counted: %d \n", VISITORS);
}