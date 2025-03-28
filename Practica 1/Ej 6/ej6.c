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
    //ungetc('a', stdin);
    printf("Error %d\n", fprintf(stdin, "c"));
    char c = getchar();
    printf("Recieved: %c\n", c);

    return 0;
}
