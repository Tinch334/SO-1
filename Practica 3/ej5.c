#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#define _GNU_SOURCE
#include <unistd.h>
#include <sys/types.h>
#include <semaphore.h>

#define N 10
pthread_cond_t barber_cond = PTHREAD_COND_INITIALIZER;
pthread_cond_t barber_done = PTHREAD_COND_INITIALIZER;
sem_t sillas_desocupadas;//hay clientes?
sem_t corte_y_pago_barber;//sincronizacion con 1 cliente
sem_t corte_y_pago_user;//sincronizacion con el barber
sem_t wait_barber;//sleepy ass
pthread_mutex_t barber = PTHREAD_MUTEX_INITIALIZER;



void me_cortan(int i){
    printf("me cortan, soy cliente n°%d \n", i);
    sleep(2);
}

void cortando(){
    printf("cortando \n");
}

void pagando(int i){
    printf("pagando, soy cliente n°%d \n",i);
}

void me_pagan(){
    printf("me pagan \n");
}


void * barbero_func(void* barber_id){
    while(1){
        sem_wait(&wait_barber);
        cortando();
        sem_post(&corte_y_pago_user);
        sem_wait(&corte_y_pago_barber);
        me_pagan();
    }
}

void* corte_maybe(void* thr_id){
    printf("cliente numero %d \n", *(int*)thr_id);
    if(sem_trywait(&sillas_desocupadas)){
        printf("0/10, aburrido, me voy >:( \n");
        return NULL;
    }


    pthread_mutex_lock(&barber);
    sem_post(&wait_barber);

    me_cortan(*(int*)thr_id);
    
    sem_wait(&corte_y_pago_user);
    sem_post(&corte_y_pago_barber);
    
    pagando(*(int*)thr_id);

    pthread_mutex_unlock(&barber);
    sem_post(&sillas_desocupadas);
    return NULL;
}

void* generar(void*) {
    int i = 0;
    while(1){
        sleep(1);
        pthread_t new;
        pthread_create(&new, NULL, corte_maybe, &i);
        i++;
    }
}

int main(){
    sem_init(&sillas_desocupadas, 0, N);
    pthread_t generador, barbero_thr; 
    pthread_create(&generador,NULL,generar,NULL);
    pthread_create(&barbero_thr,NULL,barbero_func,NULL);
    pthread_join(generador,NULL);
    pthread_join(barbero_thr,NULL);
}
