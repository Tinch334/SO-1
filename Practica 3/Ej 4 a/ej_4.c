#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>
#include "read_write.h"

#define M 5
#define N 5
#define ARRLEN 10240

int arr[ARRLEN];

RW rwLock;

void * escritor(void *arg) {
    int i;
    int num = arg - (void*)0;

    while (1) {
        sleep(random() % 2);
        printf("Escritor %d escribiendo\n", num);

        write_lock(rwLock);
        puts("locked");
        for (i = 0; i < ARRLEN; i++) {
            arr[i] = num;
        }
        write_unlock(rwLock);
    }

    return NULL;
}

void * lector(void *arg) {
    int v, i;
    int num = arg - (void*)0;
    while (1) {
        sleep(random() % 2);
        v = arr[0];

        read_lock(rwLock);
        for (i = 1; i < ARRLEN; i++) {
            if (arr[i] != v)
                break;
        }
        read_unlock(rwLock);

        if (i < ARRLEN)
            printf("Lector %d, error de lectura\n", num);
        else
            printf("Lector %d, dato %d\n", num, v);
    }

    return NULL;
}
int main() {
    pthread_t lectores[M], escritores[N];
    int i;

    rwLock = read_write_init();
    
    for (i = 0; i < M; i++)
        pthread_create(&lectores[i], NULL, lector, i + (void*)0);
    for (i = 0; i < N; i++)
        pthread_create(&escritores[i], NULL, escritor, i + (void*)0);

    pthread_join(lectores[0], NULL); /* Espera para siempre */

    return 0;
}