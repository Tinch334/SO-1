#include "slist.h"
#include <stdlib.h>
#include <assert.h>
#include <stdio.h>
#include <string.h>


SList* slist_create() {
    SList *newList = malloc(sizeof(SList));
    assert(newList != NULL);

    newList->firstNode = NULL;
    newList->lastNode = NULL;

    return newList;
}


void slist_destroy(SList *list) {
    SNode *node = list->firstNode;
    SNode *nodeToDelete;

    while (node != NULL) {
        nodeToDelete = node;
        node = node->next;
        free(nodeToDelete);
    }
}


int slist_empty(SList *list) {
    return list->firstNode == NULL;
}


SList* slist_add_end(SList *list, char *value) {
    SNode *newNode = malloc(sizeof(SNode));
    assert(newNode != NULL);

    newNode->value = strdup(value);
    //The last element of a linked list never points to anything.
    newNode->next = NULL;

    //If the list is empty then the first and last node are the same.
    if (list->lastNode == NULL)
        list->firstNode = newNode;
    else
        list->lastNode->next = newNode;

    list->lastNode = newNode;

    return list;
}


char **slist_to_list(SList *list) {
    size_t length = 0;
    for (SNode *node = list->firstNode; node != NULL ; node = node->next, length++);

    char **list_array = malloc(sizeof(char *) * length);

    SNode *node = list->firstNode;
    for (size_t i = 0; node != NULL; node = node->next, i++)
        list_array[i] = strdup(node->value);

    return list_array;
}