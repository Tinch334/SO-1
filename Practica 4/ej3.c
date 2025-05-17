#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <pthread.h>
#include <stdbool.h>
#include <omp.h>

#define ARR_LEN 500000000
double arr[ARR_LEN];
long double min;


int main(int argc, char const *argv[])
{
    srand(time(NULL));

    //Fill array
    for (long double i = 0; i < ARR_LEN; ++i)
        arr[(int)i] = 5;

    arr[566] = 1;

    min = arr[0];
    #pragma omp parallel
    {
        #pragma omp for
        for (long long int i = 0; i < ARR_LEN; i++)
        {
            #pragma omp critical
            {
                if (arr[i] < min)
                    min = arr[i];
            }
        }
    }

    printf("Main running, the minimum element is: %.2Lf\n", min);   

    return 0;
}