#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include "servidor.h"

//Sockets
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h>

//Shared memory
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>

int shmFdSize;
int shmFdList;


void quit(char *s) {
    perror(s);
    abort();
}


int fd_readline(int fd, char *buf) {
    int rc;
    int i = 0;

    while ((rc = read(fd, buf + i, 1)) > 0) {
        if (buf[i] == '\n')
            break;
        i++;
    }

    if (rc < 0)
        return rc;

    buf[i] = '\0';
    return i;
}


void handle_conn(int csock) {
    char buf[200], *arguments[3], *token;
    //We always have at least one element, the command.
    size_t arg_count = 1;

    int rc;

    void* memRefSize = mmap(0, sizeof(sizeStruct), PROT_READ | PROT_WRITE, MAP_SHARED, shmFdSize, 0);
    void* memRefList = mmap(0, ((sizeStruct *)memRefSize)->len * sizeof(keyValueStruct), PROT_READ | PROT_WRITE, MAP_SHARED, shmFdList, 0);

    while (1) {
        /* Atendemos pedidos, uno por linea */
        rc = fd_readline(csock, buf);

        if (rc < 0) {
            //Unmap shared memory.
            munmap(memRefSize, sizeof(sizeStruct));
            munmap(memRefList, ((sizeStruct *)memRefSize)->len * sizeof(keyValueStruct));

            quit("read... raro");
        }

        if (rc == 0) {
            /* Empty line, close connection */
            close(csock);

            //Unmap shared memory.
            munmap(memRefSize, sizeof(sizeStruct));
            munmap(memRefList, ((sizeStruct *)memRefSize)->len * sizeof(keyValueStruct));

            //We don't return, end child.
            exit(0);
        }

        //The first argument is the command.
        arguments[0] = strdup(strtok(buf, COMMAND_SEPARATOR));

        //Process the rest of the arguments.
        arg_count = 1;
        while ((token = strtok(NULL, COMMAND_SEPARATOR)) != NULL) {
            if (arg_count > 2) {
                write(csock, INVALID_COMMAND_ERROR, strlen(INVALID_COMMAND_ERROR));
                break;
            }

            arguments[arg_count] = strdup(token);
            arg_count++;
        }

        printf("COMM: %s - arg_count: %ld\n", arguments[0], arg_count);

        //If there were to many arguments we don't process the command.
        if (arg_count <= 3) {
            if (!strcmp(arguments[0], "PUT")) {
                size_t last_elem_pos = ((sizeStruct *)memRefSize)->lastElem;
                size_t list_len = ((sizeStruct *)memRefSize)->len;

                //If the list is too short double it's size.
                if (last_elem_pos >= list_len) {
                    list_len *= 2;
                    ftruncate(shmFdList, list_len * sizeof(keyValueStruct));
                    ((sizeStruct *)memRefSize)->len = list_len;
                }

                //Go to first empty position and store values.
                keyValueStruct *newElem = (keyValueStruct *)(memRefList + sizeof(keyValueStruct) * last_elem_pos);
                strcpy(newElem->key, arguments[1]);
                newElem->value = atoi(arguments[2]);
                //Increment element count.
                ((sizeStruct *)memRefSize)->lastElem += 1;

                write(csock, OK_COMMAND, strlen(OK_COMMAND));
            }
            else if (!strcmp(arguments[0], "DEL")) {
                size_t last_elem_pos = ((sizeStruct *)memRefSize)->lastElem;
                int found = 0;

                for (size_t i = 0; i < last_elem_pos; i++)
                {
                    keyValueStruct *elem = ((keyValueStruct *)(memRefList + sizeof(keyValueStruct) * i));

                    if (!strcmp(arguments[1], elem->key)) {
                        //To "delete" an element we set the key to NULL.
                        elem->key[0] = 0;
                        //Send "OK" to client.
                        write(csock, OK_COMMAND, strlen(OK_COMMAND));

                        //Set found flag.
                        found = 1;
                        break;
                    }
                }

                if (!found)
                    write(csock, NOT_FOUND_ERROR, strlen(NOT_FOUND_ERROR));

            }

            else if (!strcmp(arguments[0], "GET")) {
                size_t last_elem_pos = ((sizeStruct *)memRefSize)->lastElem;
                int found = 0;

                for (size_t i = 0; i < last_elem_pos; i++)
                {
                    keyValueStruct *elem = ((keyValueStruct *)(memRefList + sizeof(keyValueStruct) * i));

                    if (!strcmp(arguments[1], elem->key)) {
                        char response[100];
                        sprintf(response, "OK %d\n", elem->value);
                        write(csock, response, strlen(response));

                        //Set found flag.
                        found = 1;
                        break;
                    }
                }

                if (!found)
                    write(csock, NOT_FOUND_ERROR, strlen(NOT_FOUND_ERROR));
            }
            //Invalid command received.
            else {
                write(csock, INVALID_COMMAND_ERROR, strlen(INVALID_COMMAND_ERROR));
            }
        }
        //Invalid command received, was too long.
        else
            write(csock, INVALID_COMMAND_ERROR, strlen(INVALID_COMMAND_ERROR));

        //Free memory used by arguments.
        for (size_t i = 0; i < arg_count; i++)
            free(arguments[i]);
    }
}


void handle_clients(int lsock) {
    int csock;

    /* Wait for a connection, we don't care where it comes from */
    csock = accept(lsock, NULL, NULL);
    if (csock < 0)
        quit("accept");

    pid_t p = fork();

    if (p == 0)
        /* Child handles call*/
        handle_conn(csock);
    else {
        /* Only child needs the connection */
        close(csock);
        /* Parent waits for more connections */
        handle_clients(lsock);
    }
}


/* Creates a socket and listens on port "PORT" */
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

    /* Bindear al puerto "PORT" usando TCP, en todas las direcciones disponibles */
    rc = bind(lsock, (struct sockaddr *)&sa, sizeof sa);
    if (rc < 0)
        quit("bind");

    /* Setear en modo escucha */
    rc = listen(lsock, 10);
    if (rc < 0)
        quit("listen");

    return lsock;
}


int main() {
    int lsock;

    //Create both blocks of shared memory and set references.
    shmFdSize = shm_open(SHR_NAME_SIZE, O_CREAT | O_RDWR, PERMISOS);
    ftruncate(shmFdSize, sizeof(sizeStruct));
    shmFdList = shm_open(SHR_NAME_LIST, O_CREAT | O_RDWR, PERMISOS);
    ftruncate(shmFdList, INTIAL_LIST_LEN * sizeof(keyValueStruct));

    //Set initial values of sizeStruct.
    void* memRef = mmap(0, sizeof(sizeStruct), PROT_READ | PROT_WRITE, MAP_SHARED, shmFdSize, 0);
    ((sizeStruct *)memRef)->len = INTIAL_LIST_LEN;
    ((sizeStruct *)memRef)->lastElem = 0;
    munmap(memRef, sizeof(sizeStruct));

    //Create listening socket and wait for connections.
    lsock = mk_lsock();
    handle_clients(lsock);
}