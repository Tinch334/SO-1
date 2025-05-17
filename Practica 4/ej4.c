#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <pthread.h>
#include <stdbool.h>
#include <omp.h>
#include <math.h>
#include <sys/time.h>
#include <stdint.h>

void ompCheck(long number) {
    int divs = 0;
    int val = 1;

    printf("\nOMP\n");
    int s = ceil(sqrt(number));

    #pragma omp parallel for reduction(+: divs)
    for (long i = 2; i < s; i++)
    {
        if (number % i == 0)
            divs = divs + val;
    }

    if (divs > 0)
        printf("The number %ld is not prime\n", number);
    else
        printf("The number %ld is prime\n", number);
}

void nonOmpCheck(long number) {
    int divs = 0;

    printf("\nNon OMP\n");
    int s = ceil(sqrt(number));

    for (long i = 2; i < s; i++)
    {
            if (number % i == 0) {
                divs = 1;
                break;
            }
    }

    if (divs > 0)
        printf("The number %ld is not prime\n", number);
    else
        printf("The number %ld is prime\n", number);
}

int main(int argc, char const *argv[])
{
    long number;

    printf("Enter a number: ");
    scanf("%ld", &number);

    struct timespec start, end;
    clock_gettime(CLOCK_MONOTONIC_RAW, &start);
    ompCheck(number);
    clock_gettime(CLOCK_MONOTONIC_RAW, &end);

    uint64_t delta_us = (end.tv_sec - start.tv_sec) * 1000000 + (end.tv_nsec - start.tv_nsec) / 1000;


    clock_gettime(CLOCK_MONOTONIC_RAW, &start);
    nonOmpCheck(number);
    clock_gettime(CLOCK_MONOTONIC_RAW, &end);

    uint64_t delta_us_2 = (end.tv_sec - start.tv_sec) * 1000000 + (end.tv_nsec - start.tv_nsec) / 1000;

    printf("\n\nResult with OMP: %ld\n", delta_us);
    printf("Result without OMP: %ld\n", delta_us_2);

    return 0;
}