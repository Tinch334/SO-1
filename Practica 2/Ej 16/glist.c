#include "glist.h"
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>


GList glist_create() {
    return NULL;
}

void glist_destroy(GList list, DestroyFunction destroyFunc) {
    GNode *nodeToDelete;
    while (list != NULL) {
        nodeToDelete = list;
        list = list->next;
        destroyFunc(nodeToDelete->data);
        free(nodeToDelete);
    }
}

int glist_empty(GList list) {
    return (list == NULL);
}

GList glist_add_start(GList list, void *data, CopyFunction copyFunc) {
    GNode *newNode = malloc(sizeof(GNode));
    assert(newNode != NULL);

    newNode->next = list;
    newNode->data = copyFunc(data);

    return newNode;
}

void *glist_get_start(GList list, CopyFunction copyFunc) {
    if (glist_empty(list))
        return NULL;

    return copyFunc(list->data);
}

GList glist_delete_start(GList list, DestroyFunction destroyFunc) {
    if (glist_empty(list))
        return NULL;

    GList next = list->next;

    destroyFunc(list->data);
    free(list);

    return next;
}

int glist_find(GList list, CompareFunction compFunc, void *data) {
    int found = 0;

    for (GNode *node = list; node != NULL; node = node->next)
        if (compFunc(node->data, data) == 0)
            found = 1;

    return found;
}

GList glist_delete(GList list, CompareFunction compFunc, DestroyFunction destroyFunc, void *data, int *found) {
    *found = 0;

    if (glist_empty(list))
        return NULL;

    if (compFunc(list->data, data) == 0) {
        *found = 1;
        return glist_delete_start(list, destroyFunc);
    }
    
    GNode *prev = list;
    GNode *cur  = list->next;
    
    while (cur) {
        if (compFunc(cur->data, data) == 0) {
            /* unlink+destroy */
            prev->next = cur->next;
            destroyFunc(cur->data);
            free(cur);
            *found = 1;
            break;
        }
        prev = cur;
        cur  = cur->next;
    }

    return list;
}

void *glist_get(GList list, CompareFunction compFunc, CopyFunction copyFunc, void *data) {
    for (GNode *node = list; node != NULL; node = node->next)
        if (compFunc(node->data, data) == 0)
            return copyFunc(node->data);

    return NULL;
}

void glist_travel_extra(GList list, VisitorExtraFunction visitorFunc, void *extraValue) {
    for (GNode *node = list; node != NULL; node = node->next)
        visitorFunc(extraValue, list->data);
}