#include "socket_co.h"
#include "socket.h"
#include "coroutine.h"


using lib::coroutine::Socket;
using lib::Coroutine;

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

    connfd = libsocket_accept(sockfd);
    if (connfd < 0 && errno == EAGAIN)
    {
        wait_event(LIB_EVENT_READ);
        connfd = libsocket_accept(sockfd);
    }

    return connfd;
}

bool Socket::wait_event(int event)
{
    long id;
    Coroutine* co;
    epoll_event *ev;

    co = Coroutine::get_current();
    id = co->get_cid();

    ev = LibG.poll->events;

    ev->events = event == LIB_EVENT_READ ? EPOLLIN : EPOLLOUT;
    ev->data.u64 = touint64(sockfd, id);
    epoll_ctl(LibG.poll->epollfd,EPOLL_CTL_ADD, sockfd, ev);

    co->yield();
    return true;
}