#include "lib.h"
#include "uv.h"
#include "log.h"
#include "coroutine.h"

lib_global_t LibG;

int init_poll()
{
    size_t size;
    LibG.poll = (lib_poll_t *)malloc(sizeof(lib_poll_t));

    LibG.poll->epollfd = epoll_create(256);
    LibG.poll->ncap = 16;
    size = sizeof(struct epoll_event) * LibG.poll->ncap;
    //申请events 内存
    LibG.poll->events = (struct epoll_event *)malloc(size);
    memset(LibG.poll->events,0,size);

}

int free_poll()
{
    free(LibG.poll->events);
    free(LibG.poll);
}