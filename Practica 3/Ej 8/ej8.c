#include "barrier.h"
#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>

#define ITERS 100
#define N 10
#define W 5
float arr1[N], arr2[N];

barrier *b;

int min(int a, int b) {
    if (a > b)
        return a;

    return b;
}

void calor(float *arr, int lo, int hi, float *arr2) {
    int i;

    for (i = lo; i < hi; i++) {
        int m = arr[i];
        int l = i > 0 ? arr[i-1] : m;
        int r = i < N-1 ? arr[i+1] : m;
        arr2[i] = m + (l - m)/1000.0 + (r - m)/1000.0;
    }
}
/* Dado un array de [n], devuelve el punto de corte [i] de los [m] totales. */
static inline int cut(int n, int i, int m) {
    return i * (n/m) + min(i, n %m);
}

void * thr(void *arg) {
    int id = *(int *)arg; /* 0 <= id < W */
    int lo = cut(N, id, W), hi = cut(N, id+1, W);
    int i;
    for (i = 0; i < ITERS; i++) {
        barrier_wait(b);
        calor(arr1, lo, hi, arr2);
        printf("Waiting:%d\n", id);
        fflush(stdout);
        barrier_wait(b);
        printf("Done waiting:%d\n", id);
        calor(arr2, lo, hi, arr1);
        barrier_wait(b);
    }

    return NULL;
}


void *tst_dois(void * arg) {
    int id = *(int *)arg;
    printf("Got %d\n", id);

    while(1) {
        printf("Waiting %d\n", id);
        barrier_wait(b);
        printf("Passed %d\n", id);
        usleep(10);
    }
}

int main(int argc, char const *argv[]) {
    pthread_t threads[W];
    b = malloc(sizeof(barrier));
    barrier_init(b, W);

    int i = 0;
    for (; i < W; i++) {
        int *id = malloc(sizeof(int)); // Dynamically allocate memory for each id
        *id = i;  // Assign the correct value
        pthread_create(&threads[i], NULL, tst_dois, (void *)id);
    }

    i = 0;
    for (; i < W; i++)
        pthread_join(threads[i], NULL);

    return 0;
}
