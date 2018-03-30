#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <getopt.h>
#include "myList.h"
#include "synchronizer.h"
#include "myUtils.h"
#include "logger.h"

int main(int argc, char **argv) {
    int option_index = 0;
    char *sourcePath = NULL;
    char *destPath = NULL;
    int copyRecursively = 0;
    int fileSizeThreshold = 0;

    while ((option_index = getopt(argc, argv, "s:d:Rt:")) != -1) {
        switch (option_index) {
            case 's':
                sourcePath = optarg;
                break;
            case 'd':
                destPath = optarg;
                break;
            case 'R':
                copyRecursively = 1;
                break;
            case 't':
                fileSizeThreshold = atoi(optarg);
        }
    }

    if (sourcePath == NULL || destPath == NULL) {
        printf("Specify source and destination directories!\n");
        return 1;
    }

    struct stat pathInfo;
    int result;

    result = lstat(sourcePath, &pathInfo);
    if (result == -1 || result == 0 && !S_ISDIR(pathInfo.st_mode)) {
        printf("Given source path is not a directory or is unavailable!\n");
        return 1;
    }

    result = lstat(destPath, &pathInfo);
    if (result == -1 || result == 0 && !S_ISDIR(pathInfo.st_mode)) {
        printf("Given destination path is not a directory or is unavailable!\n");
        return 1;
    }

    if (!endsWithSlash(sourcePath))
        sourcePath = appendSlash(sourcePath);
    if (!endsWithSlash(destPath))
        destPath = appendSlash(destPath);
    
    logState("Synchronization started");
    if (copyRecursively == 1)
        synchronizeAll(sourcePath, destPath);
    else
        synchronizeFiles(sourcePath, destPath);
    logState("Synchronization finished");

    return 0;
}
