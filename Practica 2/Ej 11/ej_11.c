#include <stdio.h>
#include <unistd.h>
#include <pthread.h>

#define NRO_VISITANTES 40000
int visitantes = 0;

int mutex = 0;

int CAS(volatile int *ptr, int expected, int new_val) {
    unsigned char success;
    
    __asm__ volatile (
        "lock cmpxchg %2, %1\n\t"
        "sete %0"
        : "=q" (success),           // output: 'success' in a general register (constraint 'q').
          "+m" (*ptr),              // input/output: memory pointed by ptr; '+' indicates read-write.
          "+r" (new_val)            // input/output: new value to swap in (not strictly required as output, but used here to meet constraints).
        : "a" (expected)            // input: the expected value is loaded into the EAX register ('a').
        : "memory"                  // clobber: tells the compiler that memory is modified (prevents reordering).
    );

    //Is 1 if a swap occurred.
    return success;
}

void* molinete(void* miTurno) {
    for(int i = 0; i < NRO_VISITANTES; i++) {
        while(!CAS(&mutex, 0, 1));

        visitantes++;

        mutex = 0;

    }

    return NULL;
}

int main() {
    size_t idCount = 100;
    pthread_t idLst[idCount];
    
    for (size_t i = 0; i < idCount; i++)
    {
        pthread_create(&idLst[i], NULL, molinete, (void *)&idLst[i]);
    }

    for (size_t i = 0; i < idCount; i++)
    {
        pthread_join(idLst[i], NULL);
    }


    printf("Nro de visitantes contados: %d\n", visitantes);

    return 0;
}