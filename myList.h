#ifndef MY_LIST_H_
#define MY_LIST_H_

#include <stdlib.h>
#include <time.h>

typedef struct myNode {
    char *name;
    time_t modifiedAt;
} myNode;

typedef struct myList {
    myNode *node;
    struct myList *next;
} myList;

void addToList(myList *root, myNode *node);
myList* createList();
myNode* createNode(char *name, time_t modifiedAt);
void deleteNode(myList **root, myNode *node);
myNode* getNodeByName(myList* root, char* name);
void destroyList(myList* root);

#endif
