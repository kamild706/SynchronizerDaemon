#include <stdio.h>
#include <time.h>
#include "synchronizer.h"
#include "myList.h"
#include "myFileApi.h"
#include "myDirectoryApi.h"
#include "myUtils.h"
#include "config.h"
#include "logger.h"

static int isSynchronizationInProgress = 0;

int synchronizeFiles(char* sourcePath, char* destPath) {
    myList* destFiles = listFilesInDirectory(destPath);
    myList* sourceFiles = listFilesInDirectory(sourcePath);
   
    if (sourceFiles == NULL) {
        deleteAllFiles(destPath);

        destroyList(sourceFiles);
        destroyList(destFiles);
        return 0;
    }

    if (destFiles == NULL) {
        copyAllFiles(sourcePath, destPath);
        
        destroyList(sourceFiles);
        destroyList(destFiles);
        return 0;
    }

    while (sourceFiles != NULL) {
        myNode* node1 = sourceFiles->node;
        myNode* node2 = getNodeByName(destFiles, node1->name);
        if (node2 == NULL || difftime(node1->modifiedAt, node2->modifiedAt) > 0) {
            copyFile(sourcePath, node1->name, destPath);
        }

        deleteNode(&sourceFiles, node1);
        deleteNode(&destFiles, node2);
    }
    
    while (destFiles != NULL) {
        deleteFile(destPath, destFiles->node->name);
        deleteNode(&destFiles, destFiles->node);
    }
    
    return 0;
}

int synchronizeDirectories(char* sourcePath, char* destPath) {
    myList* sourceDirs = listDirsInDirectory(sourcePath);
    myList* destDirs = listDirsInDirectory(destPath);

    if (sourceDirs == NULL) {
        deleteAllDirs(destPath);
        
        destroyList(sourceDirs);
        destroyList(destDirs);
        return 0;
    }

    while (sourceDirs != NULL) {
        myNode* node1 = sourceDirs->node;
        myNode* node2 = getNodeByName(destDirs, node1->name);

        char* dirName = appendSlash(node1->name);
        int result = createDirIfNotExists(destPath, dirName);

        if (result == 0) {
            char* newSourcePath = concat(sourcePath, dirName);
            char* newDestPath = concat(destPath, dirName);
            synchronizeAll(newSourcePath, newDestPath);
        }
        
        deleteNode(&sourceDirs, node1);
        deleteNode(&destDirs, node2);
    }

    while (destDirs != NULL) {
        deleteDir(concat(destPath, appendSlash(destDirs->node->name)));
        deleteNode(&destDirs, destDirs->node);
    }
}

void synchronizeAll(char* sourcePath, char* destPath) {
    synchronizeFiles(sourcePath, destPath);
    synchronizeDirectories(sourcePath, destPath);
}

void synchronize(char* sourcePath, char* destPath) {
    if (isSynchronizationInProgress) {
        logState("Other synchronization is currently in progress; exiting");
        return;
    }

    isSynchronizationInProgress = 1;
    logState("Synchronization started");

    if (synchronizeRecursively == 1)
        synchronizeAll(sourcePath, destPath);
    else
        synchronizeFiles(sourcePath, destPath);

    logState("Synchronization finished");
    isSynchronizationInProgress = 0;
}
