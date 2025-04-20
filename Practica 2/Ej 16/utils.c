#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdint.h>

#include "utils.h"

//Creates a copy of the given keyValueStruct.
keyValueStruct* kvCopy(const keyValueStruct* kv) {
    keyValueStruct *newKV = malloc(sizeof(keyValueStruct));
    strcpy(newKV->key, kv->key);
    newKV->value = kv->value;

    return newKV;
}

int kvEqual(const keyValueStruct *kv1, const keyValueStruct *kv2) {
    return strcmp(kv1->key, kv2->key);
}

void kvDestroy(keyValueStruct *kv) {
    free(kv);
}

int kvHash(const keyValueStruct *kv) {
    const unsigned char *str = (const unsigned char *)kv->key;
    uint32_t hash = 5381;

    //Use djb2 on the key string.
    for (size_t i = 0; str[i] != '\0' && i < MAX_KEY_LEN; i++)
        hash = ((hash << 5) + hash) + str[i];  // hash * 33 + c

    return (int)hash;
}