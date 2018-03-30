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
#include <utime.h>
#include <time.h>

#include "myFileApi.h"
#include "myList.h"
#include "logger.h"
#include "myUtils.h"

myList* listFilesInDirectory(char* path) {
    DIR* dir = opendir(path);
    if (dir == NULL) return NULL;

    myList* list = createList();
    struct dirent* entry;
    int isEmpty = 1;
    while ((entry = readdir(dir)) != NULL) {
        char* absolutePath = concat(path, entry->d_name);
        struct stat fileInfo;
        int result = lstat(absolutePath, &fileInfo);
        // free(absolutePath);

        if (result == 0 && S_ISREG(fileInfo.st_mode)) {
            myNode* node = createNode(entry->d_name, fileInfo.st_mtim.tv_sec);
            addToList(list, node);
            isEmpty = 0;
        }
    }

    if (isEmpty) return NULL;
    return list;
}

void deleteAllFiles(char* path) {
    DIR* dir = opendir(path);
    if (dir == NULL) return;

    struct dirent* entry;
    while ((entry = readdir(dir)) != NULL) {
        deleteFile(path, entry->d_name);
    }
}

void deleteFile(char* path, char* name) {
    char* absolutePath = concat(path, name);
    struct stat fileInfo;
    int result = lstat(absolutePath, &fileInfo);

    if (result == 0 && S_ISREG(fileInfo.st_mode)) {
        result = unlink(absolutePath);
        char* message;

        if (result == 0)
            asprintf(&message, "%s has been deleted", absolutePath);
        else
            asprintf(&message, "%s %s", absolutePath, strerror(errno));
        
        logState(message);
        free(message);
    }
}

void copyAllFiles(char* source, char* dest) {
    DIR* dir = opendir(source);
    if (dir == NULL) return;

    struct dirent* entry;
    while ((entry = readdir(dir)) != NULL) {
        copyFile(source, entry->d_name, dest);
    }
}

void copyFile(char* sourcePath, char* name, char* destPath) {
    char* absolutePath = concat(sourcePath, name);

    unsigned char buffer[16];
    size_t bytesRead;
    struct stat fileInfo;
    int result = lstat(absolutePath, &fileInfo);

    if (result == 0 && S_ISREG(fileInfo.st_mode)) {
        size_t len = fileInfo.st_size;
        int input = open(absolutePath, O_RDONLY);

        char* message;
        if (input == -1) {
            asprintf(&message, "%s %s", absolutePath, strerror(errno));
            logState(message);
            free(message);
            return;
        }

        char* outputAbsolutePath = concat(destPath, name);
        int output = open(outputAbsolutePath, O_WRONLY | O_CREAT | O_TRUNC, 0666);
        if (output == -1) {
            asprintf(&message, "%s %s", outputAbsolutePath, strerror(errno));
            logState(message);
            free(message);
            return;
        }

        do {
            bytesRead = read(input, buffer, sizeof(buffer));
            write(output, buffer, bytesRead);
        }
        while (bytesRead == sizeof(buffer));

        asprintf(&message, "%s has been copied to %s", absolutePath, destPath);
        logState(message);
        free(message);

        close(input);
        close(output);
    }
}
