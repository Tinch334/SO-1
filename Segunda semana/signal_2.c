#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>º
#include <signal.h>
#include <unistd.h>

void handler_child(int sig) {
    printf("I'm the child's error handler!\n");

    exit(0);
}

int main() {
    pid_t p = fork();

    if (p == 0) {
        struct sigaction sa;
        sa.sa_flags = SA_RESTART;
        sa.sa_handler = handler_child;

        printf("Im the child, with PID: %d\n", getpid());

        sigaction(SIGINT, &sa, NULL);

        for (;;);
    }
    else {
        sleep(1);
        
        kill(p, SIGINT);
        wait(NULL);

        printf("Done with child\n");
    }

    printf("Im the parent, with PID: %d\n", getpid());
}