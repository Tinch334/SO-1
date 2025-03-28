#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <signal.h>

void sighandler(int);
void sighandler_2(int);

int main() {
    //void (*handler)(int);
    signal(SIGINT, sighandler);
    signal(SIGQUIT, sighandler_2);

    while(1) {

    }

    return 0;
}

void sighandler(int err) {
    printf("Ctrl+c pressed\n");
    //Reset handler
    signal(SIGINT, SIG_DFL);
}

void sighandler_2(int err) {
    printf("Ctrl+\\ pressed\n");
    exit(0);
}