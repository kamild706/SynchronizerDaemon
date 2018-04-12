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
#include <sys/mman.h>

#include "myFileApi.h"
#include "myList.h"
#include "logger.h"
#include "myUtils.h"
#include "config.h"

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
        free(absolutePath);

        if (result == 0 && S_ISREG(fileInfo.st_mode)) {
            myNode* node = createNode(strdup(entry->d_name), fileInfo.st_mtim.tv_sec);
            addToList(list, node);
            isEmpty = 0;
        }
    }
    closedir(dir);

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
    closedir(dir);
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
            asprintf(&message, "%s couldn't been deleted, %s", absolutePath, strerror(errno));
        
        logState(message);
        free(message);
    }
    free(absolutePath);
}

void copyAllFiles(char* source, char* dest) {
    DIR* dir = opendir(source);
    if (dir == NULL) return;

    struct dirent* entry;
    while ((entry = readdir(dir)) != NULL) {
        copyFile(source, entry->d_name, dest);
    }
    closedir(dir);
}

void copyFile(char* sourcePath, char* name, char* destPath) {
    char* absolutePath = concat(sourcePath, name);
    struct stat st;

    int result = lstat(absolutePath, &st);
    if (result == 0 && S_ISREG(st.st_mode)) {
        size_t fileSize = st.st_size;

        if (fileSize > fileSizeThreshold)
            copyFileByMMapping(sourcePath, name, destPath, fileSize);
        else
            copyFileInStandardWay(sourcePath, name, destPath);
    }
}

void copyFileByMMapping(char* sourcePath, char* name, char* destPath, size_t fileSize) {
    char* inputPath = concat(sourcePath, name);
    char* message;
    int input = open(inputPath, O_RDONLY);
    if (input == -1) {
        asprintf(&message, "%s %s", inputPath, strerror(errno));
        logState(message);

        free(message);
        free(inputPath);
        return;
    }

    char* outputPath = concat(destPath, name);
    int output = open(outputPath, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (output == -1) {
        asprintf(&message, "%s %s", outputPath, strerror(errno));
        logState(message);

        free(message);
        free(inputPath);
        free(outputPath);
        return;
    }

    char* mmappedData = mmap(NULL, fileSize, PROT_READ, MAP_PRIVATE | MAP_POPULATE, input, 0);
    if (mmappedData == MAP_FAILED) {
        asprintf(&message, "%s %s", inputPath, strerror(errno));
        logState(message);

        free(message);
        free(inputPath);
        free(outputPath);
        return;
    }
    int result = write(output, mmappedData, fileSize);

    if (result == -1) {
        asprintf(&message, "%s couldn't been copied (mmapped) to %s, %s", inputPath, destPath, strerror(errno));
    } else {
        asprintf(&message, "%s has been copied (mmapped) to %s", inputPath, destPath);
    }

    logState(message);

    free(message);
    free(inputPath);
    free(outputPath);

    close(input);
    close(output);

    munmap(mmappedData, fileSize);
}

void copyFileInStandardWay(char* sourcePath, char* name, char* destPath) {
    char* message;

    char* absolutePath = concat(sourcePath, name);
    int input = open(absolutePath, O_RDONLY);
    if (input == -1) {
        asprintf(&message, "%s couldn't been copied, %s", absolutePath, strerror(errno));
        logState(message);

        free(message);
        free(absolutePath);
        return;
    }

    char* outputAbsolutePath = concat(destPath, name);
    int output = open(outputAbsolutePath, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (output == -1) {
        asprintf(&message, "%s couldn't been updated, %s", outputAbsolutePath, strerror(errno));
        logState(message);

        free(message);
        free(absolutePath);
        free(outputAbsolutePath);
        return;
    }

    unsigned char buffer[16];
    size_t bytesRead;
    int result = -1;
    do {
        bytesRead = read(input, buffer, sizeof(buffer));
        result = write(output, buffer, bytesRead);
    }
    while (bytesRead == sizeof(buffer));

    close(input);
    close(output);

    if (result == -1) {
        asprintf(&message, "%s couldn't been copied to %s, %s", absolutePath, destPath, strerror(errno));
    } else {
        asprintf(&message, "%s has been copied to %s", absolutePath, destPath);
    }

    logState(message);
    
    free(message);
    free(absolutePath);
    free(outputAbsolutePath);
}
