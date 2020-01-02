#include "socket_co.h"
#include "socket.h"
#include "coroutine.h"
#include "log.h"


using lib::coroutine::Socket;
using lib::Coroutine;

Socket::Socket(int fd)
{
    sockfd = fd;
    libsocket_set_nonblock(sockfd);
}

Socket::Socket(int domain, int type, int protocol)
{
    sockfd = libsocket_create(domain, type, protocol);
    if (sockfd < 0)
    {
        return;
    }
    libsocket_set_nonblock(sockfd);
}

int Socket::bind(int type, char *host, int port)
{
    return libsocket_bind(sockfd, type, host, port);
}

int Socket::listen()
{
    return libsocket_listen(sockfd);
}

int Socket::accept()
{
    int connfd;

    do{
        connfd = libsocket_accept(sockfd);
        printf("%d",connfd);
    } while (connfd < 0 && errno == EAGAIN && wait_event(LIB_EVENT_READ));

    return connfd;
}

bool Socket::wait_event(int event)
{
    long id;
    Coroutine* co;
    epoll_event *ev;

    co = Coroutine::get_current();
    id = co->get_cid();
    if(!LibG.poll)
    {
        init_poll();
    }
    ev = LibG.poll->events;

    ev->events = event == LIB_EVENT_READ ? EPOLLIN : EPOLLOUT;
    ev->data.u64 = touint64(sockfd, id);
    epoll_ctl(LibG.poll->epollfd,EPOLL_CTL_ADD, sockfd, ev);
    (LibG.poll->event_num)++;
    co->yield();

    if(epoll_ctl(LibG.poll->epollfd,EPOLL_CTL_DEL,sockfd,NULL) < 0 ){
        libWarn("error has occurred errno %d %s",errno,strerror(errno));
        return false;
    }
    return true;
}
ssize_t Socket::recv(void *buf,size_t len)
{
    int ret;
    do{
        ret = libsocket_recv(sockfd,buf,len,0);
    } while (ret < 0 && errno == EAGAIN && wait_event(LIB_EVENT_READ));

    return ret;
}
ssize_t Socket::send(const void *buf, size_t len)
{
    int ret;

    do{
        ret = libsocket_send(sockfd, buf, len, 0);
    } while (ret < 0 && errno == EAGAIN && wait_event(LIB_EVENT_WRITE));

    return ret;
}
int Socket::close()
{
    return libsocket_close(sockfd);
}
Socket::~Socket()
{
}

//分配全局recv write 数据以缓冲区
//因为每个协程是同步执行，也就是同一时间只会有一个协程读写全局缓冲区，不会发生冲突
int Socket::init_read_buffer()
{
    if (!read_buffer)
    {
        try{
            read_buffer  = new char[65536];
        }catch(const std::bad_alloc& e){
            libError("%s",e.what());
        }
//        read_buffer = (char *)malloc(65536);
//        if (read_buffer == NULL)
//        {
//            return -1;
//        }
        read_buffer_len = 65536;
    }

    return 0;
}

int Socket::init_write_buffer()
{
    if (!write_buffer)
    {
        try{
            write_buffer = new char[65536];
        }catch(const std::bad_alloc& e)
        {
            libError("%s",e.what());
        }
//        write_buffer = (char *)malloc(65536);
//        if (write_buffer == NULL)
//        {
//            return -1;
//        }
        write_buffer_len = 65536;
    }

    return 0;
}