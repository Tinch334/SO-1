#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <fcntl.h>
#include <pthread.h>

int GLOBAL_INT = 0;

void *fun (void* v){
    printf("Thread v:%d\n",*(int*)v);

    GLOBAL_INT = 1;

    //Store 5 in %rax.
    int reg = 5;
    asm ("movl %0, %%eax" :: "r"(reg));

    int *ptr = malloc(sizeof(int));
    *ptr = 7;

    pthread_exit(ptr);
}

/*
En base a las pruebas vemos que solo los valores en el heap y las variables globales se comparten entre threads.
*/
int main(){
    pthread_t thread;
    int v = 2;
    int* ptr = NULL;

    printf("Thread creation here, global is: %d\n", GLOBAL_INT);
    pthread_create(&thread, NULL, fun, &v);
    
    //Wait for thread to finish.
    pthread_join(thread, (void**)&ptr);

    //Read value of %rax and store it in "reg".
    int reg;
    asm("movl %%eax, %0" : "=r"(reg) :);

    printf("Main running, global is: %d - Register is: %d - Ptr is: %d\n", GLOBAL_INT, reg, *ptr);
}