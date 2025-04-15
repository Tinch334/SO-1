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
a- Si, pues el problema de cambio de contexto en medio de una operacion no depende de la cantidad de nucleos.
b- Siguen siendo necesarias las barreras de memoria, para prevenir el reordenamiento de instrucciones.
c- Si, pues declarar una funcion como inline no implica que el codigo se inlinee, GCC solo hace esto al optimizar o al usar
__attribute__((always_inline)).
d- No se actualizan los caches?
e- Ahora funciona, porque la funcion usa lock :).
*/

static inline void incl(int *p) {
    asm("incl %0" : "+m"(*p) : : "memory");
}

static inline void incl2(int *p) {
    asm("lock; incl %0" : "+m"(*p) : : "memory");
}

void *contar_thread_1(void* v){
    for(int i = 0; i < *((int *)v); i++)
        incl(&VISITORS);

    pthread_exit(NULL);
}

void *contar_thread_2(void* v){
    for(int i = 0; i < *((int *)v); i++)
        incl(&VISITORS);

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