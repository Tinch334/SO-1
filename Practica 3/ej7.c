#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>

#define B 2
#define K (2*B)
volatile int s, r, buf[B];

static inline int diff() { return (K + s - r) % K; }
void * prod(void *_arg) {
int cur = 0;
while (1) {
while (diff() == B)
;
buf[s % B] = cur++;
s = (s+1) % K;
}
}
void * cons(void *_arg) {
int cur;
while (1) {
    while (diff() == 0)
    ;
    cur = buf[r % B];
    r = (r+1) % K;
    printf("Leı %d\n", cur);
    }
    }

//NO funciona para mas de dos procesos.
int main() {
    srand(time(NULL));

    pthread_t s1, s2;

    pthread_create(&s1, NULL, prod, NULL);
    pthread_create(&s2, NULL, cons, NULL);

    while(1);   
    return 0;
}