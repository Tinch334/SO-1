#include "recursive_mutex.h"
#include <unistd.h>
#include <sys/types.h>



recinfo r;
int visitantes = 0;
int NRO_VISITANTES = 10;

void* molinete(void* v) {
    pid_t p = gettid();

    for(int i = 0; i < NRO_VISITANTES; i++) {
        printf("Reached with ID: %d\n", p);
        mutexLockR(r, p);
      
        visitantes++;
        printf("Visitantes: %d - ID: %d\n", visitantes, p);
    }

    for(int i = 0; i < NRO_VISITANTES; i++)
        mutexUnlockR(r, p);

    return NULL;
}


int main() {
    r = mutexInitR();
    int num = 10;

    pthread_t id_lst[num];
    
    for (size_t i = 1; i <= 10; i++)
        id_lst[i] = (pthread_t)i;

    for (size_t i = 1; i <= num; i++)
        pthread_create(&id_lst[i], NULL, molinete, NULL);

    for (size_t i = 1; i <= num; i++)
        pthread_join(id_lst[i], NULL);
}
