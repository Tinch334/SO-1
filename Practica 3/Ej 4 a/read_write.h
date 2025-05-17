#ifndef __READ_WRITE_H__
#define __READ_WRITE_H__

#include <pthread.h>
#include <stdbool.h>

typedef struct {
    size_t readerCount;
    bool readRequest;

    pthread_cond_t canRead;
    pthread_cond_t canWrite;
    
    pthread_mutex_t mutex;
}  _read_write;
typedef _read_write *RW;

RW read_write_init();

void read_lock(RW rw);

void read_unlock(RW rw);

void write_lock(RW rw);

void write_unlock(RW rw);

#endif