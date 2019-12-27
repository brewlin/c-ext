#include "socket.h"
#include "log.h"

int libsocket_create(int domain,int type,int protocol)
{
    int sock;
    sock = socket(domain,type,protocol);
    if(sock < 0 )
    {
        libWarn("Error has occurred: (errno %d) %s", errno, strerror(errno));
    }
    return sock;
}

int libsocket_bind(int sock,int type,char *host,int port)
{
    int ret;
    struct sockaddr_in servaddr;

    if(type == LIB_SOCK_TCP)
    {
        bzero(&servaddr, sizeof(servaddr));
        inet_aton(host,&(servaddr.sin_addr));

        servaddr.sin_family = AF_INET;
        servaddr.sin_port = htons(port);
        ret = bind(sock,(struct sockaddr *)&servaddr, sizeof(servaddr));
        if(ret < 0 ){
            return -1;
        }
    }else{return -1;}
    return ret;
}

int libsocket_listen(int sock)
{
    int ret;

    ret = listen(sock, 512);
    if (ret < 0)
    {
        php_printf("Error has occurred: (errno %d) %s", errno, strerror(errno));
    }
    return ret;
}


int libsocket_accept(int sock)
{
    int connfd;
    struct sockaddr_in sa;
    socklen_t len;

    len = sizeof(sa);
    connfd = accept(sock, (struct sockaddr *)&sa, &len);

    return connfd;
}


ssize_t libsocket_recv(int sock,void *buf,size_t len,int flag)
{
    ssize_t ret;
    ret = recv(sock,buf,len,flag);
    if(ret <0 )
    {
        //warn
    }
    return ret;
}

ssize_t libsocket_send(int sock, const void *buf, size_t len, int flag)
{
    ssize_t ret;

    ret = send(sock, buf, len, flag);
    if (ret < 0)
    {
        php_printf("Error has occurred: (errno %d) %s", errno, strerror(errno));
    }
    return ret;
}

int libsocket_set_nonblock(int sock)
{
    int flags;

    flags = fcntl(sock, F_GETFL, 0);
    if (flags < 0) {
        libWarn("Error has occurred: (errno %d) %s", errno, strerror(errno));
        return -1;
    }
    flags = fcntl(sock, F_SETFL, flags | O_NONBLOCK);
    if (flags < 0) {
        libWarn("Error has occurred: (errno %d) %s", errno, strerror(errno));
        return -1;
    }
    return 0;
}
int libsocket_close(int fd)
{
    int ret;

    ret = close(fd);
    if (ret < 0)
    {
        libWarn("Error has occurred: (errno %d) %s", errno, strerror(errno));
    }
    return ret;
}
