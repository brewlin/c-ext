#include "log.h"

char lib_debug[LIB_DEBUG_MSG_SIZE];
char lib_trace[LIB_TRACE_MSG_SIZE];
char lib_warn[LIB_WARN_MSG_SIZE];
char lib_error[LIB_ERROR_MSG_SIZE];

void libLog_put(int level, char *cnt)
{
    const char *level_str;
    char date_str[LIB_LOG_DATE_STRLEN];
    char log_str[LIB_LOG_BUFFER_SIZE];
    int n;
    time_t t;
    struct tm *p;

    switch (level) {
        case LIB_LOG_DEBUG:
            level_str = "DEBUG";
            break;
        case LIB_LOG_NOTICE:
            level_str = "NOTICE";
            break;
        case LIB_LOG_ERROR:
            level_str = "ERROR";
            break;
        case LIB_LOG_WARNING:
            level_str = "WARNING";
            break;
        case LIB_LOG_TRACE:
            level_str = "TRACE";
            break;
        default:
            level_str = "INFO";
            break;
    }

    t = time(NULL);
    p = localtime(&t);
    snprintf(date_str, LIB_LOG_DATE_STRLEN, "%d-%02d-%02d %02d:%02d:%02d", p->tm_year + 1900, p->tm_mon + 1, p->tm_mday, p->tm_hour, p->tm_min, p->tm_sec);

    n = snprintf(log_str, LIB_LOG_BUFFER_SIZE, "[%s]\t%s\t%s\n", date_str, level_str, cnt);
    if (write(STDOUT_FILENO, log_str, n) < 0) {
        printf("write(log_fd, size=%d) failed. Error: %s[%d].\n", n, strerror(errno), errno);
    }
}