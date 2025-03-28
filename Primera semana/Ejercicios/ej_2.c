#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>

int main(int argc, char const *argv[])
{
    const char *binary = argv[1];
    const int time = atoi(argv[2]);

    while (1) {
        pid_t p = fork();
        printf("Child PID: %d\n", p);

        if (p == 0) {
            execl(binary, binary, NULL);
        }

        sleep(time);
    }
}