//
// Created by Administrator on 2019/12/19 0019.
//
#include "channel.h"

//关闭通道
void close_channel(fd_t *fd)
{
    if (close(fd[0]) == -1) {
        php_print("close() channel failed");
    }

    if (close(fd[1]) == -1) {
        php_print("close() channel failed");
    }
}