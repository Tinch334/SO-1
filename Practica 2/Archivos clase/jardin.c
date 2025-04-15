#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <fcntl.h>
#include <pthread.h>

int VISITORS = 0;
//int flag = 0; - intento 1
int turn = 1;//1 is thread_1's turn and 2 is thread_2's turn, 0 is no turn assigned 


void *contar_thread_1(void* v){
    for(int i = 0; i < *((int *)v); i++) {
        //Lock

        while (turn ==2)
        {;}

        //CRITICAL SECTION: Not atomic, requires three assembly instructions
        VISITORS++;
        printf("Counted 1: %d\n", VISITORS);

        //Unlock
        turn = 2;
    }

    pthread_exit(NULL);
}

void *contar_thread_2(void* v){
    for(int i = 0; i < *((int *)v); i++) {
        //Lock

        while (turn == 1)
        {;}

        //CRITICAL SECTION: Not atomic, requires three assembly instructions
        VISITORS++;
        printf("Counted 2: %d\n", VISITORS);

        //Unlock
        turn = 1;
    }

    pthread_exit(NULL);
}

int main(){
    pthread_t thread, thread_2;
    int v = 200000, w = 200000;
    printf("Thread creation here\n");
    pthread_create(&thread, NULL, contar_thread_1, &v);
    pthread_create(&thread_2, NULL, contar_thread_2, &w);
    
    //Wait for thread to finish.
    pthread_join(thread, NULL);
    pthread_join(thread_2, NULL);

    printf("Main running, counted: %d \n", VISITORS);
}