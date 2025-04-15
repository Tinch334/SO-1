#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <fcntl.h>
#include <pthread.h>
#include <assert.h>

volatile int x = 0;
volatile int y = 0;

//La siguiente asercion no puede fallar, pues si y = 1 esto implica que ya se ejecuto la linea que hace que x = 123.
void *wr(void *arg) {
    x = 123;
    y = 1;
}

void *rd(void *arg) {
    while (!y)
        ;
    assert(x == 123);
}