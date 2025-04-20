#ifndef __UTILS_H__
#define __UTILS_H__

#include "servidor_epoll.h"
#include "hash_table.h"

keyValueStruct* kvCopy(const keyValueStruct* kv);

int kvEqual(const keyValueStruct *kv1, const keyValueStruct *kv2);

void kvDestroy(keyValueStruct *kv);

int kvHash(const keyValueStruct *kv);

#endif/* __UTILS_H__ */