#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <fcntl.h>
#include <pthread.h>


int VISITORS = 0;


void *contar_thread(void* v){
    for(int i = 0; i < *((int *)v); i++)
        VISITORS++;

    pthread_exit(NULL);
}

/*
Al compilar usando -O3 el conteo es correcto en algunos casos, comparado con casi ninguno al no usar optimizacion. Creemos que es porque al
optimizar hay menos lineas de codigo en la zona critica, por lo tanto es menos probable que ocurra un cambio de contexto cuando se esta en ella.
*/

int main(){
    pthread_t thread, thread_2;
    int v = 200000, w = 200000;
    printf("Thread creation here\n");
    pthread_create(&thread, NULL, contar_thread, &v);
    pthread_create(&thread_2, NULL, contar_thread, &w);
    
    //Wait for thread to finish.
    pthread_join(thread, NULL);
    pthread_join(thread_2, NULL);

    printf("Main running, counted: %d \n", VISITORS);
}