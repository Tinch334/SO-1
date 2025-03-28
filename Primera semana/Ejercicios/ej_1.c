#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

int main(int argc, char const *argv[])
{
    char contents[1024];
    int file = open("./example.txt", O_RDONLY);

    ssize_t bytes = read(file, contents, 1024);

    fprintf(stdout, "Bytes read: %ld\n", bytes);
    puts(contents);

    close(file);

    return 0;
}