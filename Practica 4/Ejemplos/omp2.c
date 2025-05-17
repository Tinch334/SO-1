#include <stdio.h>
#include <omp.h>


int main(){
    #pragma omp parallel
    {
        #pragma omp sections
        {
            #pragma omp section
            {
                printf("Hello from thread %d (section 1)\n", omp_get_thread_num());
            }
            #pragma omp section
            {
                printf("Hello from thread %d (section 2)\n", omp_get_thread_num());
            }
            #pragma omp section
            {
                printf("Hello from thread %d (section 3)\n", omp_get_thread_num());
            }
        }
    }   
    return 0;
}