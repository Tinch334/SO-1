#include <stdio.h>
#include <unistd.h>
#include <pthread.h>

pthread_mutex_t a = PTHREAD_MUTEX_INITIALIZER;

void * foo(void *_arg) {

    pthread_mutex_lock(&a);
    printf("Foo!\n");
    sleep(1);
    pthread_mutex_unlock(&a);

    return NULL;
}

/*
Vemos que inmediatamente se imprimen ambas strings, esto ocurre ya que mutex a y b son distintos, en particular sus direcciones son distintas.
Entonces un lock de b no afecta uno de a
*/
int main() {
    pthread_t th;
    pthread_create(&th, NULL, foo, NULL);
    pthread_mutex_t b = a;

    pthread_mutex_lock(&b);
    printf("Bar!\n");
    sleep(1);
    pthread_mutex_unlock(&b);

    pthread_join(th, NULL);

    return 0;
}