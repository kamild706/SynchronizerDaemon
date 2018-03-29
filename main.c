#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <errno.h>
#include <getopt.h>
#include "myList.h"
#include "directoryFunctions.h"
#include "synchronizer.h"

int main(int argc, char **argv) {
    int option_index = 0;
    char *sourceDir = NULL;
    char *destDir = NULL;
    int copyRecursively = 0;

    while ((option_index = getopt(argc, argv, "s:d:R")) != -1) {
        switch (option_index) {
            case 's':
                sourceDir = optarg;
                break;
            case 'd':
                destDir = optarg;
                break;
            case 'R':
                copyRecursively = 1;
                break;
        }
    }

    if (sourceDir == NULL || destDir == NULL) {
        printf("Specify source and destination directories!\n");
        return 1;
    }
    struct stat sdir;
    struct stat ddir;

    lstat(sourceDir, &sdir);
    if (!S_ISDIR(sdir.st_mode)) {
        printf("Given source path is not a directory!\n");
        return 1;
    }

    lstat(destDir, &ddir);
    if (!S_ISDIR(ddir.st_mode)) {
        printf("Given destination path is not a directory!\n");
        return 1;
    }

    synchronize(sourceDir, destDir);

    return 0;
}
