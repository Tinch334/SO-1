#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>

#define N_FILOSOFOS 5
#define ESPERA 500000


/*
int sem_init(sem_t *sem, int pshared, unsigned int value);
int sem_destroy(sem_t *sem);
int sem_wait(sem_t *sem);
int sem_post(sem_t *sem);
*/
sem_t eating_sem;

void pensar(int i) {
    printf("Filosofo %d pensando...\n", i);
    usleep(random() % ESPERA);
}

void comer(int i) {
    printf("Filosofo %d comiendo...\n", i);
    usleep(random() % ESPERA);
}

void tomar_tenedores() {
    sem_wait(&eating_sem);
}

void dejar_tenedores() {
    sem_post(&eating_sem);
}

void * filosofo(void *arg)
{
    int i = arg - (void*)0;
    while (1) {
        tomar_tenedores();
        comer(i);
        dejar_tenedores();
        pensar(i);
    }
}

int main() {
    pthread_t filo[N_FILOSOFOS];
    int i;

    if (sem_init(&eating_sem, 0, N_FILOSOFOS - 2) == -1) {
        fprintf(stderr, "sem_init\n");
        exit(-1);
    }

    for (i = 0; i < N_FILOSOFOS; i++)
        pthread_create(&filo[i], NULL, filosofo, i + (void*)0);

    pthread_join(filo[0], NULL);
    sem_destroy(&eating_sem);

    return 0;
}