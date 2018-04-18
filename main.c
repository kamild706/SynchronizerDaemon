#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <getopt.h>
#include <errno.h>
#include <signal.h>
#include <time.h>

#include "synchronizer.h"
#include "myUtils.h"
#include "logger.h"
#include "config.h"

char *sourcePath = NULL;
char *destPath = NULL;
int sleepTime = 5 * 60;

void handler(int signum) {
    logState("Synchronization invoked by signal");
    synchronize(sourcePath, destPath);
}

int runDaemon() {
    pid_t pid, sid;
    pid = fork();
    if (pid < 0) {
        exit(EXIT_FAILURE);
    }
    if (pid > 0) {
        exit(EXIT_SUCCESS);
    }

    umask(0);

    sid = setsid();
    if (sid < 0) {
        logState(strerror(errno));
        exit(EXIT_FAILURE);
    }

    if ((chdir("/")) < 0) {
        logState(strerror(errno));
        exit(EXIT_FAILURE);
    }

    printf("PID %d\n", sid);

    close(STDIN_FILENO);
    close(STDOUT_FILENO);
    close(STDERR_FILENO);

    struct timespec req, rem;
    while (1) {
        logState("Daemon awoken");
        synchronize(sourcePath, destPath);
        logState("Daemon falls asleep");
        
        req.tv_sec = sleepTime;
        req.tv_nsec = 0L;
        while (nanosleep(&req, &rem) == -1) {
            req.tv_sec = rem.tv_sec;
            req.tv_nsec = rem.tv_nsec;
        }
    }
}

int main(int argc, char** argv) {
    int option_index = 0;
    int copyRecursively = 0;
    long thresholdValue = 0;
    int pauseTime = 0;

    while ((option_index = getopt(argc, argv, "s:d:Rt:p:")) != -1) {
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
                thresholdValue = atoi(optarg);
                break;
            case 'p':
                pauseTime = atoi(optarg);
                break;
            case '?':
                printf("Error: unknown argument was passed: %c\n", optopt);
                return 1;
            default:
                abort();
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
    
    if (thresholdValue > 0)
        fileSizeThreshold = thresholdValue;
    if (copyRecursively == 1)
        synchronizeRecursively = 1;
    if (pauseTime > 0)
        sleepTime = pauseTime;

    signal(SIGUSR1, handler);
    runDaemon();

    return 0;
}
