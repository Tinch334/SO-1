#ifndef __SLIST_H__
#define __SLIST_H__


#include <stddef.h>


typedef void (*VisitorFunction) (int value);


typedef struct _SNode {
    char *value;
    struct _SNode *next;
} SNode;


typedef struct {
   SNode *firstNode;
   SNode *lastNode; 
} SList;


//Returns an empty list.
SList* slist_create();

//Destroys a list.
void slist_destroy(SList *list);

//Determines if the list is empty.
int slist_empty(SList *list);

//Adds an element to the end of the list.
SList* slist_add_end(SList *list, char *value);

//Returns the linked list as a normal list, with a "NULL" as the last element.
char **slist_to_list(SList *list);

#endif /* __SLIST_H__ */