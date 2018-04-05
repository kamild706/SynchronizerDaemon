#include <stdio.h>
#include <string.h>
#include "myList.h"

void addToList(myList* root, myNode* node) {
    if (root->node == NULL) {
        root->node = node;
        root->next = NULL;
        return;
    }

    while (root->next != NULL) {
        root = root->next;
    }
    root->next = malloc(sizeof(myList));
    root->next->node = node;
    root->next->next = NULL;  
}

myList* createList() {
    myList* list = malloc(sizeof(myList));
    list->node = NULL;
    list->next = NULL;
    return list;
}

myNode* createNode(char* name, time_t modifiedAt) {
    myNode* node = malloc(sizeof(myNode));
    node->name = name;
    node->modifiedAt = modifiedAt;

    return node;
}

void deleteNode(myList** root, myNode* node) {
    if (node == NULL) return;

    myList* temp = *root, *prev;
    if (temp != NULL && temp->node == node) {
        *root = temp->next;
        free(node);
        free(temp);
        return;
    }

    while (temp != NULL && temp->node != node) {
        prev = temp;
        temp = temp->next;
    }

    if (temp == NULL) return;
    prev->next = temp->next;
    free(node);
    free(temp);
}

myNode* getNodeByName(myList* root, char* name) {
    if (root == NULL) return NULL;

    while (root != NULL) {
        if (strcmp(root->node->name, name) == 0) {
            return root->node;
        }
        root = root->next;
    }

    return NULL;
}

void destroyList(myList* root) {
    myList* tmp = root;
    while (root != NULL) {
        free(root->node);
        tmp = root->next;
        free(root);
        root = tmp;
    }
}
