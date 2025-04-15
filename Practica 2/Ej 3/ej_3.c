#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <fcntl.h>
#include <pthread.h>
#include <assert.h>

int x = 0, y = 0, a = 0, b = 0;

void *foo(void *arg) {
    x = 1;
    a = y;

    return NULL;
}

void *bar(void *arg) {
    y = 1;
    b = x;

    return NULL;
}

/*
Como esta garantizado que ambas funciones terminan pues sabemos que al menos se va a ejecutar la linea x = 1 o y = 1, luego como sabemos que se
van a ejecutar el resto de las lineas eventualmente a = 1 o b = 1.
*/
int main() {
    pthread_t t0, t1;

    pthread_create(&t0, NULL, foo, NULL);
    pthread_create(&t1, NULL, bar, NULL);

    pthread_join(t0, NULL);
    pthread_join(t1, NULL);

    assert (a || b);

    return 0;
}