#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>

//Sockets
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h>

//Threads
#include <pthread.h>

//Epoll
#include <sys/epoll.h>

#define MAX_EVENTS 10

int COUNT = 0;

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

int epollFD;

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


//Returns -1 if connection closed.
int handle_conn(int csock)
{
    char buf[200];
    int rc;

    
    /* Atendemos pedidos, uno por linea */
    rc = fd_readline(csock, buf);

    if (rc < 0) {
        close(csock);
        quit("read... raro");
    }

    if (rc == 0) {
        /* linea vacia, se cerró la conexión */
        close(csock);
        return -1;
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
    else {
        char reply[20];

        sprintf(reply, "EINVAL\n");
        write(csock, reply, strlen(reply));
    }

    return 0;
}


void *handle_thread(void *l)
{
    int nfds, lsock, conn_sock;
    struct sockaddr addr;
    socklen_t addrlen;
    struct epoll_event ev, events[MAX_EVENTS];

    lsock = *((int *)l);

    for(;;) {
        nfds = epoll_wait(epollFD, events, MAX_EVENTS, -1);
        if (nfds == -1)
            quit("epoll_wait");

        for (int n = 0; n < nfds; ++n) {
            if (events[n].data.fd == lsock) {
                conn_sock = accept(lsock, (struct sockaddr *)&addr, &addrlen);
                if (conn_sock == -1) 
                    quit("accept");

                //Add FD to monitoring as oneshot. This is done to prevent a situation where not all the data is read, causing the unread
                //data to be read by another thread.
                ev.events = EPOLLIN | EPOLLONESHOT;
                ev.data.fd = conn_sock;
                if (epoll_ctl(epollFD, EPOLL_CTL_ADD, conn_sock, &ev) == -1)
                    quit("epoll_ctl: conn_sock");
            }
            else {
                //Read FD to monitoring list if the connection wasn't closed.
                if (handle_conn(events[n].data.fd) != -1) {
                    ev.events = EPOLLIN | EPOLLONESHOT;
                    ev.data.fd = events[n].data.fd;
                    if (epoll_ctl(epollFD, EPOLL_CTL_MOD ,events[n].data.fd, &ev) == -1)
                        quit("epoll_ctl: conn_sock");
                }
            }

        }
    }
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
    int lsock, num = 2;
    struct epoll_event ev;
    lsock = mk_lsock();

    epollFD = epoll_create1(0);

    //Create epoll monitor
    if (epollFD == (-1))
        quit("epoll_create1");

    ev.events = EPOLLIN;
    ev.data.fd = lsock;
    if (epoll_ctl(epollFD, EPOLL_CTL_ADD, lsock, &ev) == -1)
        quit("epoll_ctl: listen_sock");

    pthread_t id_lst[num];
    
    for (size_t i = 1; i <= num; i++)
        id_lst[i] = (pthread_t)i;
    
    for (size_t i = 1; i <= num; i++)
        pthread_create(&id_lst[i], NULL, handle_thread, (void *)&lsock);
    
    for (size_t i = 1; i <= num; i++)
        pthread_join(id_lst[i], NULL);
}