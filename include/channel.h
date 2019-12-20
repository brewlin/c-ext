//
// Created by brewlin on 2019/12/19 0019.
//
#ifndef C_EXT_CHANNEL_H
#define C_EXT_CHANNEL_H

#include "lib.h"

#define CMD_TEST1 1
#define CMD_TEST2 2
#define CMD_TEST3 3



typedef struct {
    //channel 之间传递 int 型数据
    uint_t command;
    //发送命令方的进程id
    pid_t pid;

    //全局进程数组中的序号
    uint_t slot;

    fd_t fd;
}channel_t;


int_t write_channel(socket_t s, channel_t *ch, size_t size);
int_t read_channel(socket_t s, channel_t *ch, size_t size);
void close_channel(fd_t *fd);

#endif //C_EXT_CHANNEL_H
