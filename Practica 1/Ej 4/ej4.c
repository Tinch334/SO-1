#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <signal.h>
#include <string.h>

int main(int argc, char const *argv[])
{
    printf("PID: %d\n", getpid());

    const size_t size = 10000000;
    int *ptr = malloc(sizeof(int) * size);
    size_t c = 0;

    while (1) {
        const size_t size = 10000000;
        int *ptr = malloc(sizeof(int) * size);
        size_t c = 0;

        while (c < size) {
                ptr[c] = c;
                c++;
        }
    }

    printf("Done\n");
    getchar();

    return 0;
}
