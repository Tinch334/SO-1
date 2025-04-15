#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

//Sockets
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h>

//Threads
#include <pthread.h>

int COUNT = 0;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

void quit(char *s)
{
    perror(s);
    abort();
}


int fd_readline(int fd, char *buf)
{
    int rc;
    int i = 0;

    /*
     * Leemos de a un caracter (no muy eficiente...) hasta
     * completar una línea.
     */
    while ((rc = read(fd, buf + i, 1)) > 0) {
        if (buf[i] == '\n')
            break;
        i++;
    }

    if (rc < 0)
        return rc;

    buf[i] = 0;
    return i;
}


void *handle_conn(void *arg)
{
    int csock = *((int *)arg);
    char buf[200];
    int rc;

    while (1) {
        /* Atendemos pedidos, uno por linea */
        rc = fd_readline(csock, buf);

        if (rc < 0) {
            close(csock);
            quit("read... raro");
        }

        if (rc == 0) {
            /* linea vacia, se cerró la conexión */
            close(csock);
            //We don't return, end child.
            exit(0);
            //We don't return, end child.
        }

        if (!strcmp(buf, "NUEVO")) {
            char reply[20];

            // No hace falta usar try_lock solo hay un recurso, no puede haber deadlock.
            pthread_mutex_lock(&mutex);
            sprintf(reply, "%d\n", COUNT);
            COUNT++;
            pthread_mutex_unlock(&mutex);

            write(csock, reply, strlen(reply));

        }
    }
}


void handle_clients(int lsock)
{
    int csock;

    /* Esperamos una conexión, no nos interesa de donde viene */
    csock = accept(lsock, NULL, NULL);
    if (csock < 0)
        quit("accept");

    pthread_t t;
    pthread_create(&t, NULL, handle_conn, (void *)&csock);

    /* El main espera mas llamadas */
    handle_clients(lsock);
}


/* Crea un socket de escucha en puerto 4040 TCP */
int mk_lsock()
{
    struct sockaddr_in sa;
    int lsock;
    int rc;
    int yes = 1;

    /* Crear socket */
    lsock = socket(AF_INET, SOCK_STREAM, 0);
    if (lsock < 0)
        quit("socket");

    /* Setear opción reuseaddr... normalmente no es necesario */
    if (setsockopt(lsock, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof yes) == 1)
        quit("setsockopt");

    sa.sin_family = AF_INET;
    sa.sin_port = htons(4040);
    sa.sin_addr.s_addr = htonl(INADDR_ANY);

    /* Bindear al puerto 4040 TCP, en todas las direcciones disponibles */
    rc = bind(lsock, (struct sockaddr *)&sa, sizeof sa);
    if (rc < 0)
        quit("bind");

    /* Setear en modo escucha */
    rc = listen(lsock, 10);
    if (rc < 0)
        quit("listen");

    return lsock;
}


int main()
{
    int lsock;

    lsock = mk_lsock();
    handle_clients(lsock);
}