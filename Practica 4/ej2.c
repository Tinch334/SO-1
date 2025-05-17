#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <pthread.h>
#include <stdbool.h>
#include <omp.h>

#define ARR_LEN 500000000
double arr[ARR_LEN];
long double suma = 0;


int main(int argc, char const *argv[])
{
    //Fill array
    for (long double i = 0; i < ARR_LEN; ++i)
        arr[(int)i] = 1;

    #pragma omp parallel
    {
        #pragma omp for
        for (long long int i = 0; i < ARR_LEN; i++)
        {
            #pragma omp critical
            {
                suma += arr[i];
            }
        }
    }

    printf("Main running, the sum is: %.2Lf\n", suma);

    return 0;
}