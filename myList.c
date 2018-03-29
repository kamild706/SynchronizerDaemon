#include <stdio.h>
#include <string.h>
#include "myList.h"

void addToList(myList* root, myNode* node) {
	if (root->node == NULL) {
		root->node = node;
	}
	else {
		while (root->next != NULL) {
			root = root->next;
		}
		root->next = malloc(sizeof(myList));
		root->next->node = node;
	}
}

myList* createList() {
	myList* list = malloc(sizeof(myList));
	return list;
}

myNode* createNode(char* name, time_t modifiedAt) {
	myNode* node = malloc(sizeof(myNode));
	node->name = name;
	node->modifiedAt = modifiedAt;

	return node;
}

/*void deleteNode(myList** root, myNode* node) {
	if (node == NULL) return;

	if ((*root)->node == node) {
		myList* next = (*root)->next;
		free(node);
		free(*root);
		*root = next;
	}
	else {
		myList* prev;
		while ((*root)->next != NULL) {
			prev = *root;
			*root = (*root)->next;
			if ((*root)->node == node) {
				free(node);
				prev->next = (*root)->next; 
			}
		}
	}
}*/

void deleteNode(myList** root, myNode* node) {
	myList* temp = *root, *prev;
	if (temp != NULL && temp->node == node) {
		*root = temp->next;
		free(temp);
		return;
	}

	while (temp != NULL && temp->node != node) {
		prev = temp;
		temp = temp->next;
	}

	if (temp == NULL) return;
	prev->next = temp->next;
	free(temp);
}

myNode* getNodeByName(myList* root, char* name) {
	if (root == NULL) return NULL;

	do {
		if (strcmp(root->node->name, name) == 0) {
			return root->node;
		}
		root = root->next;
	}
	while (root != NULL);

	return NULL;
}
