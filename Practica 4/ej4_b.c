#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <inttypes.h>
#include <omp.h>

int ompCheck(long number) {
    int found = 0;
    long s = (long)ceil(sqrt((double)number));

    #pragma omp parallel for
    for (long i = 2; i < s; i++)
    {
        if (number % i == 0) {
            #pragma omp atomic write
            found = 1;
        }
    }

    return found;
}

int nonOmpCheck(long number) {
    int found = 0;

    long s = (long)ceil(sqrt((double)number));

    for (long i = 2; i < s; i++)
    {
        if (number % i == 0) {
            found = 1;
            break;
        }
    }

    return found;
}

int main(int argc, char const *argv[])
{
    long number;

    printf("Enter a number: ");
    scanf("%ld", &number);

    struct timespec start, end;
    clock_gettime(CLOCK_MONOTONIC_RAW, &start);
    int res1 = ompCheck(number);
    clock_gettime(CLOCK_MONOTONIC_RAW, &end);

    uint64_t delta_us = (end.tv_sec - start.tv_sec) * 1000000 + (end.tv_nsec - start.tv_nsec) / 1000;


    clock_gettime(CLOCK_MONOTONIC_RAW, &start);
    int res2 = nonOmpCheck(number);
    clock_gettime(CLOCK_MONOTONIC_RAW, &end);

    uint64_t delta_us_2 = (end.tv_sec - start.tv_sec) * 1000000 + (end.tv_nsec - start.tv_nsec) / 1000;

    printf("\n\nResult with OMP: %d - Took: %ld\n", res1, delta_us);
    printf("Result without OMP: %d - Took: %ld\n", res2, delta_us_2);

    return 0;
}