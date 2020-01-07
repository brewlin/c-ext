//
// Created by Administrator on 2019/12/26 0026.
//

#ifndef C_EXT_SOCKET_H
#define C_EXT_SOCKET_H

#include "lib.h"
#include "php_lib.h"
enum libsocket_type
{
    LIB_SOCK_TCP          =  1,
    LIB_SOCK_UDP          =  2,
};



int libsocket_create(int domain,int type,int protocol);

int libsocket_bind(int sock,int type,char *host,int port);

int libsocket_listen(int sock,int backlog);

int libsocket_accept(int sock);

ssize_t libsocket_recv(int sock,void *buf,size_t len,int flag);

ssize_t libsocket_send(int sock, const void *buf, size_t len, int flag);

int libsocket_close(int fd);
int libsocket_set_nonblock(int sock);

#endif //C_EXT_SOCKET_H
