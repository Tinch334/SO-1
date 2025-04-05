#ifndef __SERVER_H__
#define __SERVER_H__

#define MAX_KEY_LEN 50

typedef struct {
    char key[MAX_KEY_LEN];
    int value;
} keyValueStruct;

typedef struct {
    size_t len;
    size_t lastElem;
} sizeStruct;

#define SHR_NAME_SIZE "SHARED_MEM_SIZE"
#define SHR_NAME_LIST "SHARED_MEM_LIST"
#define PORT 3942
#define PERMISOS 0666
#define INTIAL_LIST_LEN 8

#define COMMAND_SEPARATOR " "
#define OK_COMMAND "OK\n"
#define NOT_FOUND_ERROR "NOTFOUND\n"
#define INVALID_COMMAND_ERROR "EINVAL\n"

#endif/* __SERVER_H__ */