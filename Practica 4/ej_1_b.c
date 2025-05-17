#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <pthread.h>
#include <stdbool.h>
#include <omp.h>

#define VISITOR_NUM 10000

#define N 4

//Global visitor counter.
volatile int VISITORS = 0;

void contar_thread(){
    int id = omp_get_thread_num();

    for(int i = 0; i < VISITOR_NUM; i++) {
        //Lock
        #pragma omp critical
        {
            VISITORS++;
            printf("Counted %d: %d\n", id, VISITORS);
        }
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