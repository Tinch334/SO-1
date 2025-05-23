#ifndef __GLIST_H__
#define __GLIST_H__

typedef void (*DestroyFunction)(void *data);
typedef void *(*CopyFunction)(void *data);
typedef void (*VisitorExtraFunction) (void *extra, void *data);
//Returns a negative value if data1 < data2, 0 if they are equal and a positive value if data1 > data2.
typedef int (*CompareFunction)(void *data1, void *data2);

typedef struct _GNode {
    void *data;
    struct _GNode *next;
} GNode;

typedef GNode *GList;

//Returns an empty list.
GList glist_create();

//Destroys the given list.
void glist_destroy(GList list, DestroyFunction destroyFunc);

//Returns 1 if the given list is empty.
int glist_empty(GList list);

//Adds the given element to the start of the list.
GList glist_add_start(GList list, void *data, CopyFunction copyFunc);

//Returns the element at the start of the list, if it's empty returns NULL.
void *glist_get_start(GList list, CopyFunction copyFunc);

//Destroys the first element in the list.
GList glist_delete_start(GList list, DestroyFunction destroyFunc);

//Returns 1 if the given value is in the list, returns 0 otherwise.
int glist_find(GList list, CompareFunction comp, void *data);

//Deletes the given value from the list, returns 1 if the given value is in the list, returns 0 otherwise.
GList glist_delete(GList list, CompareFunction compFunc, DestroyFunction destroyFunc, void *data, int *found);

//Returns the given element if it exists, otherwise returns NULL.
void *glist_get(GList list, CompareFunction compFunc, CopyFunction copyFunc, void *data);

//Travels the list calling the given function with the additional value on each element.
void glist_travel_extra(GList list, VisitorExtraFunction visit, void *extraValue);

#endif/* __GLIST_H__ */