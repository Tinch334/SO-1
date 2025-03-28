#include<stdio.h>
#include<unistd.h>

int main(){
    printf("I'm a dummy program\n");
    printf("PID: %d\n\n", getpid());
    return 0;
}
