#ifndef MY_DIRECTORY_API_H_
#define MY_DIRECTORY_API_H_

#include "myList.h"

myList* listDirsInDirectory(char* dir);
int createDirIfNotExists(char* path, char* dirName, char* basePath);
void deleteAllDirs(char* path);
void deleteDir(char* path);

#endif
