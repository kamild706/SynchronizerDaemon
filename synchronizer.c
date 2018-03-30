#include <stdio.h>
#include <time.h>
#include "synchronizer.h"
#include "myList.h"
#include "directoryFunctions.h"

void myPrint(myList* root) {
    if (root == NULL) return;
    printf("%s\n", root->node->name);
    myPrint(root->next);
}

int synchronize(char* sourceDir, char* destDir) {
    myList* sourceFiles = listFilesInDirectory(sourceDir);
    myList* destFiles = listFilesInDirectory(destDir);

    if (sourceFiles == NULL) {
        deleteAllFiles(destDir);
        return 0;
    }

    if (destFiles == NULL) {
        copyAllFiles(sourceDir, destDir);
        return 0;
    }

    do {
        myNode* node1 = sourceFiles->node;
        myNode* node2 = getNodeByName(destFiles, node1->name);
        if (node2 == NULL || difftime(node1->modifiedAt, node2->modifiedAt) > 0) {
            copyFile(sourceDir, node1->name, destDir);
        }

        deleteNode(&sourceFiles, node1);
        deleteNode(&destFiles, node2);
    }
    while (sourceFiles != NULL);

    if (destFiles != NULL) {
        do {
            deleteFile(destDir, destFiles->node->name);
            deleteNode(&destFiles, destFiles->node);
        }
        while (destFiles != NULL);
    }
}
