#include <stdio.h>
#include <omp.h>

int main()
{
    int i = 10;
    int j = 20;
    #pragma omp parallel private(i)
    {
        // variable i is not initialized
        printf("thread %d: i = %d - j = %d\n", omp_get_thread_num(), i, j);
        j++;
        i = 1000;
    }
    printf("private i = %d - public j = %d\n", i, j);

    return 0;
}
 