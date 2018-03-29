#include <stdio.h>
#include <unistd.h>
#include <syslog.h>
#include "logger.h"

void logState(char* message) {
    openlog("synchronizerDaemon", LOG_PID | LOG_CONS, LOG_USER);
    syslog(LOG_INFO, message);
    closelog();
}
