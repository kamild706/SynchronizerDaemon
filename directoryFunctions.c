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

#include "directoryFunctions.h"
#include "myList.h"
#include "logger.h"

char* concat(const char *s1, const char *s2) {
    char *result = malloc(strlen(s1)+strlen(s2)+1);
    strcpy(result, s1);
    strcat(result, s2);
    return result;
}

myList* listFilesInDirectory(char* dirPath) {
	DIR* dir = opendir(dirPath);

	myList* list = createList();
	struct dirent* entry;
	int isEmpty = 1;
	while ((entry = readdir(dir)) != NULL) {
		char* fullPath = concat(dirPath, entry->d_name);
		struct stat fileInfo;
		lstat(fullPath, &fileInfo);
		// free(fullPath);

		if (S_ISREG(fileInfo.st_mode)) {
			myNode* node = createNode(entry->d_name, fileInfo.st_mtim.tv_sec);
			addToList(list, node);
			isEmpty = 0;
		}
	}

	if (isEmpty == 1) return NULL;
	return list;
}

void deleteAllFiles(char* dirPath) {
	DIR* dir = opendir(dirPath);

	struct dirent* entry;
	while ((entry = readdir(dir)) != NULL) {
		deleteFile(dirPath, entry->d_name);
	}
}

void deleteFile(char* path, char* name) {
	char* fullPath = concat(path, name);
	struct stat fileInfo;
	lstat(fullPath, &fileInfo);

	if (S_ISREG(fileInfo.st_mode)) {
		int result = unlink(fullPath);
		char* message;

		if (result == 0) {
			asprintf(&message, "%s has been deleted", fullPath);
		}
		else {
			asprintf(&message, "%s %s", fullPath, strerror(errno));
		}
		
		logState(message);
		free(message);
	}
}

void copyAllFiles(char* source, char* dest) {
	DIR* dir = opendir(source);

	struct dirent* entry;
	while ((entry = readdir(dir)) != NULL) {
		copyFile(source, entry->d_name, dest);
	}
}

void copyFile(char* sourcePath, char* name, char* destPath) {
	char* fullPath = concat(sourcePath, name);

	unsigned char buffer[16];
	size_t bytesRead;
	struct stat fileInfo;
	lstat(fullPath, &fileInfo);

	if (S_ISREG(fileInfo.st_mode)) {
		size_t len = fileInfo.st_size;
		int input = open(fullPath, O_RDONLY);

		char* message;
		if (input == -1) {
			asprintf(&message, "%s %s", fullPath, strerror(errno));
			logState(message);
			free(message);
			return;
		}

		char* outputFullPath = concat(destPath, name);
		int output = open(outputFullPath, O_WRONLY | O_CREAT, 0666);
		if (output == -1) {
			asprintf(&message, "%s %s", outputFullPath, strerror(errno));
			logState(message);
			free(message);
			return;
		}

		do {
	    	bytesRead = read(input, buffer, sizeof(buffer));
	    	write(output, buffer, bytesRead);
		}
	    while (bytesRead == sizeof(buffer));

	    asprintf(&message, "%s has been copied to %s", fullPath, destPath);
		logState(message);
		free(message);

	    /*struct utimbuf* times;
	    time_t x = fileInfo.st_mtim.tv_sec;
	    times->actime = x;
	    times->modtime = x;

	    utime(outputFullPath, times);*/
	    close(input);
	    close(output);
	}
}
