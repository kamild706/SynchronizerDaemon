#ifndef MY_FILE_API_H_
#define MY_FILE_API_H_

#include "myList.h"

myList* listFilesInDirectory(char *dir);
void deleteAllFiles(char* dir);
void copyAllFiles(char* source, char* dest);
void deleteFile(char* path, char* name);
void copyFile(char* sourcePath, char* name, char* destPath);
int copyFileInStandardWay(char* sourcePath, char* name, char* destPath);
int copyFileByMMapping(char* sourcePath, char* name, char* destPath, size_t fileSize);

#endif
