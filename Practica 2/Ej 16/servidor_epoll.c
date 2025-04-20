#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>

#include "servidor_epoll.h"
#include "hash_table.h"
#include "utils.h"

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

HashTable hTable;

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
    char buf[200], *arguments[3], *token;
    int rc;
    size_t arg_count;

    /* Atendemos pedidos, uno por linea */
    rc = fd_readline(csock, buf);

    if (rc < 0)
        quit("read... raro");

    if (rc == 0) {
        /* Empty line, close connection */
        close(csock);

        //Return 0 to avoid FD being watched.
        return 0;
    }

    //The first argument is the command.
    arguments[0] = strdup(strtok(buf, COMMAND_SEPARATOR));

    //Process the rest of the arguments.
    arg_count = 1;
    while ((token = strtok(NULL, COMMAND_SEPARATOR)) != NULL) {
        arguments[arg_count] = strdup(token);
        arg_count++;
    }

    printf("COMM: %s - arg_count: %ld\n", arguments[0], arg_count);

    //The first argument is always the key, check that it's not too long, furthermore check that we don't have too many arguments.
    if (strlen(arguments[1]) > MAX_KEY_LEN - 1 || arg_count > 3) {
        write(csock, INVALID_COMMAND_ERROR, strlen(INVALID_COMMAND_ERROR));
        return 1;
    }

    if (!strcmp(arguments[0], "PUT") && arg_count == 3) {
        keyValueStruct kv;
        strcpy(kv.key, arguments[1]);
        kv.value = atoi(arguments[2]);
            
        pthread_mutex_lock(&mutex);

        //Check that the element isn't in the hash table.
        if (!htable_contains(hTable, &kv)) {
            htable_insert(hTable, &kv);
            write(csock, OK_COMMAND, strlen(OK_COMMAND));
        }
        else
            write(csock, REPETITION_ERROR, strlen(REPETITION_ERROR));

        pthread_mutex_unlock(&mutex);
    }
    else if (!strcmp(arguments[0], "DEL") && arg_count == 2) {
        keyValueStruct kv;
        strcpy(kv.key, arguments[1]);
        //Value doesn't matter, equaltiy function doesn't check it.
        kv.value = 0;

        pthread_mutex_lock(&mutex);

        if (htable_delete(hTable, &kv))
            write(csock, OK_COMMAND, strlen(OK_COMMAND));
        else
            write(csock, NOT_FOUND_ERROR, strlen(NOT_FOUND_ERROR));

        pthread_mutex_unlock(&mutex);
    }

    else if (!strcmp(arguments[0], "GET") && arg_count == 2) {
        keyValueStruct kv, *kvRes;
        strcpy(kv.key, arguments[1]);
        //Value doesn't matter, equaltiy function doesn't check it.
        kv.value = 0;            
        kvRes = malloc(sizeof(keyValueStruct));

        pthread_mutex_lock(&mutex);
        kvRes = htable_get(hTable, &kv);

        if (kvRes == NULL)
            write(csock, NOT_FOUND_ERROR, strlen(NOT_FOUND_ERROR));
        else {
            char response[150];
            sprintf(response, "OK %d\n", kvRes->value);
            write(csock, response, strlen(response));
        }
        pthread_mutex_unlock(&mutex);

        free(kvRes);
    }
    //Invalid command received.
    else
        write(csock, INVALID_COMMAND_ERROR, strlen(INVALID_COMMAND_ERROR));

    //Free memory used by arguments.
    for (size_t i = 0; i < arg_count; i++)
        free(arguments[i]);

    return 1;
}


void *handle_thread(void *l) {
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
                //Readd FD to monitoring list if the connection wasn't closed.
                if (handle_conn(events[n].data.fd)) {
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
int mk_lsock() {
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
    sa.sin_port = htons(PORT);
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
    int lsock, num = 4;
    struct epoll_event ev;
    lsock = mk_lsock();

    epollFD = epoll_create1(0);

    hTable = htable_create(50, (CopyFunctionHash)kvCopy, (CompareFunctionHash)kvEqual, (DestroyFunctionHash)kvDestroy, (HashFunction)kvHash);

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