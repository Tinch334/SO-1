#include<stdio.h>
#include<unistd.h>

int main(){
    printf("Soy execl.c!\nPID: %d\n", getpid());

    const char* prog = "./ejemplo";

    //execl toma primero el nombre del programa y luego sus argumentos. Un programa en C tiene su nombre como primer argumento.
    execl(prog, prog, NULL);

    return 0;
}
