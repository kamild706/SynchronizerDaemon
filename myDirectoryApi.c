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
        free(absolutePath);

        if (result == 0 && S_ISDIR(dirInfo.st_mode)) {
            myNode* node = createNode(strdup(entry->d_name), dirInfo.st_mtim.tv_sec);
            addToList(list, node);
            isEmpty = 0;
        }
    }
    closedir(dir);

    if (isEmpty == 1) return NULL;
    return list;
}

int createDirIfNotExists(char* path, char* dirName, char* basePath) {
    char* absolutePath = concat(path, dirName);

    DIR* dir = opendir(absolutePath);
    if (dir) {
        closedir(dir);
        return 0;
    }

    struct stat st;
    char* sourcePath = concat(basePath, dirName);
    lstat(sourcePath, &st);
    free(sourcePath);

    char* message;
    if (errno == ENOENT) {
        int result = mkdir(absolutePath, st.st_mode);
        if (result == 0) {
            chown(absolutePath, st.st_uid, st.st_gid);
            asprintf(&message, "%s has been created", absolutePath);
            logState(message);
            free(message);
            free(absolutePath);
            return 0;
        }
        else {
            asprintf(&message, "%s couldn't been created, %s", absolutePath, strerror(errno));
            logState(message);
            free(message);
            free(absolutePath);
            return -1;
        }
    }
    else {
        asprintf(&message, "%s couldn't been created, %s", absolutePath, strerror(errno));
        logState(message);
        free(message);
        free(absolutePath);
        return -1;
    }
}

void deleteAllDirs(char* path) {
    DIR* dir = opendir(path);
    if (dir == NULL) {
        char* message;
        asprintf(&message, "Directories from %s couldn't been deleted, %s", path, strerror(errno));
        logState(message);
        free(message);
        return;
    }

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
        free(absolutePath);
    }
    closedir(dir);
}

void deleteDir(char* path) {
    DIR* dir = opendir(path);
    if (dir == NULL)  {
        char* message;
        asprintf(&message, "Directory %s couldn't been deleted, %s", path, strerror(errno));
        logState(message);
        free(message);
        return;
    }

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
        free(absolutePath);
    }
    closedir(dir);

    int result = rmdir(path);

    char* message;
    if (result == 0)
        asprintf(&message, "%s has been deleted", path);
    else
        asprintf(&message, "%s couldn't been deleted, %s", path, strerror(errno));
    
    logState(message);
    free(message);
}
