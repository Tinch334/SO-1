#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <fcntl.h>
#include <pthread.h>

int VISITORS = 0;
int flag[2] = {0,0}; //Indicates intention to execute
int turn = 0; //Indicates who's turn it is.

/*
    La asignacion de variables es atomica en x86 pero no en ARM. Estamos viendo codigo
    multiplataforma
*/

void *contar_thread_1(void* v){
    for(int i = 0; i < *((int *)v); i++) {
        //Lock
        flag[0] = 1; //thread_1 wants turn.
        asm("mfence");
        turn = 2; //Give turn to other thread.
        
        while (flag[1] == 1 && turn == 2)
        {;}

        //CRITICAL SECTION: Not atomic, requires three assembly instructions
        VISITORS++;

        //Unlock
        flag[0] = 0;
    }

    pthread_exit(NULL);
}

void *contar_thread_2(void* v){
    for(int i = 0; i < *((int *)v); i++) {
        //Lock

        flag[1] = 1; //thread_1 wants turn
        asm("mfence");
        turn = 1;
        while (flag[0] == 1 && turn == 1)
        {;}
        
        //CRITICAL SECTION: Not atomic, requires three assembly instructions
        VISITORS++;

        //Unlock
        flag[1] = 0;
    }

    pthread_exit(NULL);
}

/*
La instruccion mfence puede ubicarse entre la bandera y el turno, pues siempre asumo que es el turno del otro proceso.
*/
int main(){
    pthread_t thread, thread_2;
    int v = 2000000, w = 2000000;
    printf("Thread creation here\n");
    pthread_create(&thread, NULL, contar_thread_1, &v);
    pthread_create(&thread_2, NULL, contar_thread_2, &w);
    
    //Wait for thread to finish.
    pthread_join(thread, NULL);
    pthread_join(thread_2, NULL);

    printf("Main running, counted: %d \n", VISITORS);
}