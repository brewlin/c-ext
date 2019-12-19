//
// Created by Administrator on 2019/12/19 0019.
//
#include "channel.h"
#include "php_lib.h"
//关闭通道
void close_channel(fd_t *fd)
{
    if (close(fd[0]) == -1) {
        php_printf("close() channel failed");
    }

    if (close(fd[1]) == -1) {
        php_printf("close() channel failed");
    }
}