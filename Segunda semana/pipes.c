#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <signal.h>
#include <string.h>


int main() {
    int fd[2];

    pipe(fd);
    pid_t p = fork();

    if (p == 0) {
        char readBuffer[1024], c;
        int ptr = 0;
        //Close unused writing pipline.
        close(fd[1]);

        int sizeread = 1;
        while (sizeread != 0 && c != '\0') {
            sizeread = read(fd[0], &c, 1);
            printf("Child read: %c\n", c);
        }

    }
    else {
        char writeBuffer[1024];
        fgets(writeBuffer, 1024, stdin);
        //char writeBuffer[] = "Hola capo";

        //Close unused reading pipline.
        close(fd[0]);
        write(fd[1], writeBuffer, strlen(writeBuffer) + 1);

        wait(NULL);
    }
}