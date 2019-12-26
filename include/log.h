//
// Created by Adminilibrator on 2019/12/26 0026.
//

#ifndef C_EXT_LOG_H
#define C_EXT_LOG_H


#include "lib.h"

#define LIB_OK 0
#define LIB_ERR -1

#define LIB_LOG_BUFFER_SIZE 1024
#define LIB_LOG_DATE_STRLEN  64

#define LIB_DEBUG_MSG_SIZE 512
#define LIB_TRACE_MSG_SIZE 512
#define LIB_WARN_MSG_SIZE 512
#define LIB_ERROR_MSG_SIZE 512

extern char lib_debug[LIB_DEBUG_MSG_SIZE];
extern char lib_trace[LIB_TRACE_MSG_SIZE];
extern char lib_warn[LIB_WARN_MSG_SIZE];
extern char lib_error[LIB_ERROR_MSG_SIZE];

#define libDebug(str, ...)                                                         \
    snprintf(lib_debug, LIB_DEBUG_MSG_SIZE, "%s: " str " in %s on line %d.", __func__, ##__VA_ARGS__, __FILE__, __LINE__); \
    libLog_put(LIB_LOG_DEBUG, lib_debug);

#define libTrace(str, ...)                                                         \
    snprintf(lib_trace, LIB_TRACE_MSG_SIZE, "%s: " str " in %s on line %d.", __func__, ##__VA_ARGS__, __FILE__, __LINE__); \
    libLog_put(LIB_LOG_TRACE, lib_trace);

#define libWarn(str, ...)                                                         \
    snprintf(lib_error, LIB_ERROR_MSG_SIZE, "%s: " str " in %s on line %d.", __func__, ##__VA_ARGS__, __FILE__, __LINE__); \
    libLog_put(LIB_LOG_WARNING, lib_error);

#define libError(str, ...)                                                         \
    snprintf(lib_error, LIB_ERROR_MSG_SIZE, "%s: " str " in %s on line %d.", __func__, ##__VA_ARGS__, __FILE__, __LINE__); \
    libLog_put(LIB_LOG_ERROR, lib_error); \
    exit(-1);

enum libLog_level
{
    LIB_LOG_DEBUG = 0,
    LIB_LOG_TRACE,
    LIB_LOG_INFO,
    LIB_LOG_NOTICE,
    LIB_LOG_WARNING,
    LIB_LOG_ERROR,
};

void libLog_put(int level, char *cnt);




#endif //C_EXT_LOG_H
