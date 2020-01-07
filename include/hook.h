//
// Created by Administrator on 2020/1/7 0007.
//

#ifndef C_EXT_HOOK_H
#define C_EXT_HOOK_H

#include "php_lib.h"
#include "socket_co.h"
#include "socket.h"

//hook sleep function
void hook_sleep();

//hook socket_stream
void hook_socket_stream();

#endif //C_EXT_HOOK_H
