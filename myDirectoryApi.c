#define _GNU_SOURCE
#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <dirent.h>
#include <utime.h>
#include <time.h>

#include "myDirectoryApi.h"
#include "myFileApi.h"
#include "myList.h"
#include "logger.h"
#include "myUtils.h"

myList* listDirsInDirectory(char* dirPath) {
    DIR* dir = opendir(dirPath);
    if (dir == NULL) return NULL;

    myList* list = createList();
    struct dirent* entry;
    int isEmpty = 1;
    while ((entry = readdir(dir)) != NULL) {
        if (strcmp(entry->d_name, ".") == 0) continue;
        if (strcmp(entry->d_name, "..") == 0) continue;

        char* absolutePath = concat(dirPath, entry->d_name);
        struct stat dirInfo;
        int result = lstat(absolutePath, &dirInfo);
        // free(absolutePath);

        if (result == 0 && S_ISDIR(dirInfo.st_mode)) {
            myNode* node = createNode(entry->d_name, dirInfo.st_mtim.tv_sec);
            addToList(list, node);
            isEmpty = 0;
        }
    }

    if (isEmpty == 1) return NULL;
    return list;
}

int createDirIfNotExists(char* path, char* dirName) {
    char* absolutePath = concat(path, dirName);

    DIR* dir = opendir(absolutePath);
    if (dir) {
        closedir(dir);
        return 0;
    }

    char* message;
    if (errno == ENOENT) {
        int result = mkdir(absolutePath, ACCESSPERMS);
        if (result == 0) {
            asprintf(&message, "%s has been created", absolutePath);
            logState(message);
            return 0;
        }
        else {
            asprintf(&message, "%s couldn't been created, %s", absolutePath, strerror(errno));
            logState(message);
            return -1;
        }
    }
    else {
        asprintf(&message, "%s couldn't been created, %s", absolutePath, strerror(errno));
        logState(message);
        return -1;
    }
}

void deleteAllDirs(char* path) {
    DIR* dir = opendir(path);
    if (dir == NULL) return;

    struct dirent* entry;

    while ((entry = readdir(dir)) != NULL) {
        if (strcmp(entry->d_name, ".") == 0) continue;
        if (strcmp(entry->d_name, "..") == 0) continue;

        struct stat entryInfo;
        char* absolutePath = concat(path, entry->d_name);
        int result = lstat(absolutePath, &entryInfo);

        if (result == 0 && S_ISDIR(entryInfo.st_mode)) {
            deleteDir(appendSlash(absolutePath));
        }
    }
}

void deleteDir(char* path) {
    DIR* dir = opendir(path);
    if (dir == NULL) return;

    struct dirent* entry;
    while ((entry = readdir(dir)) != NULL) {
        if (strcmp(entry->d_name, ".") == 0) continue;
        if (strcmp(entry->d_name, "..") == 0) continue;

        char* absolutePath = concat(path, entry->d_name);
        struct stat entryInfo;
        int result = lstat(absolutePath, &entryInfo);

        if (result == 0 && S_ISDIR(entryInfo.st_mode)) {
            deleteDir(appendSlash(absolutePath));
        }
        else if (result == 0 && S_ISREG(entryInfo.st_mode)) {
            deleteFile(path, entry->d_name);
        }
    }

    int result = rmdir(path);

    char* message;
    if (result == 0)
        asprintf(&message, "%s has been deleted", path);
    else
        asprintf(&message, "%s %s", path, strerror(errno));
    
    logState(message);
    free(message);
}
