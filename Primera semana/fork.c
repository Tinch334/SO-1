#include<stdio.h>
#include<unistd.h>
#include<sys/types.h>

int main(){
    printf("Hello\n");
    int a = 10;

    pid_t pid1 = fork();
    pid_t pid2 = fork();

    printf("pid1: %d pid2: %d\n",pid1,pid2);
    return 0;
}