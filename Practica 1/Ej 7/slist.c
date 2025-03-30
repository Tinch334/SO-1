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
    newList->length = 0;

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
    list->length = list->length + 1;

    return list;
}


char **slist_to_list(SList *list) {
    //Allocate extra space for "NULL".
    char **list_array = malloc(sizeof(char *) * (list->length + 1));

    SNode *node = list->firstNode;
    for (size_t i = 0; node != NULL; node = node->next, i++)
        list_array[i] = strdup(node->value);

    //Terminate list with "NULL", this is to follow execvp calling convention.
    list_array[list->length] = NULL;

    return list_array;
}


size_t slist_length(SList *list) {
    return list->length;
}