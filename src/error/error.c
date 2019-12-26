#include "error.h"
#include "log.h"

const char* lib_strerror(int code)
{
    switch (code)
    {
        case LIB_ERROR_SESSION_CLOSED_BY_SERVER:
            return "Session closed by socket";
            break;
        case LIB_ERROR_SESSION_CLOSED_BY_CLIENT:
            return "Session closed by client";
            break;
        default:
            snprintf(lib_error, sizeof(lib_error), "Unknown error: %d", code);
            return lib_error;
            break;
    }
}