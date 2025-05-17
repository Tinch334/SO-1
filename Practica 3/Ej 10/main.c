#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include "channel.h"

#define M 5
#define N 5
#define SZ 8

/*
 * El buffer guarda punteros a enteros, los
 * productores los consiguen con malloc() y los
 * consumidores los liberan con free()
 */
channel *communicationChannel;

void enviar(int p)
{
    chan_write(communicationChannel, p);

    return;
}

int recibir()
{
    return chan_read(communicationChannel);
}

void * prod_f(void *arg)
{
    int id = arg - (void*)0;
    while (1) {
        sleep(random() % 3);

        int p = random() % 100;
        printf("Productor %d: produje %d\n", id, p);
        enviar(p);
    }
    return NULL;
}

void * cons_f(void *arg)
{
    int id = arg - (void*)0;
    while (1) {
        sleep(random() % 3);

        int p = recibir();
        printf("Consumidor %d: obtuve %d\n", id, p);
    }
    return NULL;
}

int main()
{
    pthread_t productores[M], consumidores[N];
    int i;

    communicationChannel = malloc(sizeof(channel));
    channel_init(communicationChannel);

    for (i = 0; i < M; i++)
        pthread_create(&productores[i], NULL, prod_f, i + (void*)0);

    for (i = 0; i < N; i++)
        pthread_create(&consumidores[i], NULL, cons_f, i + (void*)0);

    pthread_join(productores[0], NULL); /* Espera para siempre */
    return 0;
}