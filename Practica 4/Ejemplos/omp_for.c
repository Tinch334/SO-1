#include <stdio.h>
#include <omp.h>

int main()
{
    int a[100];

    #pragma omp parallel
    {
        #pragma omp for
        for (size_t i = 0; i < 100; i++)
        {
            int id = omp_get_thread_num();
            a[i] = i*2;
            printf("ID: %d - a[%d] = %d\n", id, i, a[i]);
        }
        
    }

    return 0;
}
