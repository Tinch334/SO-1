#ifndef __SERVER_EPOLL_H__
#define __SERVER_EPOLL_H__

#define MAX_KEY_LEN 50

typedef struct {
    char key[MAX_KEY_LEN];
    int value;
} keyValueStruct;


#define PORT 3942

#define COMMAND_SEPARATOR " "
#define OK_COMMAND "OK\n"
#define NOT_FOUND_ERROR "NOTFOUND\n"
#define REPETITION_ERROR "REPETITION\n"
#define INVALID_COMMAND_ERROR "EINVAL\n"

#endif/* __SERVER_EPOLL_H__ */