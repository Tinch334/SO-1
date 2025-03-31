#include <stdlib.h>
#include <stdio.h>
#include <signal.h>

int denom = 0;

void handler(int s) { printf("ouch!\n"); denom = 1; }

int main() {
    int r;
    //Al usar signal solo esta definido que pasa si salimos del programa o usamos longjump, cosas que no se hacen. Luego este programa
    //contiene undefined behaviour. En este caso se repite infinitamente.
    signal(SIGFPE, handler);
    r = 1 / denom;
    printf("r = %d\n", r);
    return 0;
}