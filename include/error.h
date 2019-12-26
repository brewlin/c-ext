//
// Created by Administrator on 2019/12/26 0026.
//

#ifndef C_EXT_ERROR_H
#define C_EXT_ERROR_H

#include "lib.h"

enum libErrorCode
{
    /**
     * connection error
     */
    LIB_ERROR_SESSION_CLOSED_BY_SERVER = 1001,
    LIB_ERROR_SESSION_CLOSED_BY_CLIENT
};
const char* lib_strerror(int code);


#endif //C_EXT_ERROR_H
