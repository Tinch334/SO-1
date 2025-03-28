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
    char c;

    while(1) {
        fscanf(stdin, "%c", &c);
        fprintf(stdout, "Got: %c\n", c);
    }

    return 0;
}
