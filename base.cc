#include "include/lib.h"
#include "uv.h"
#include "include/log.h"
#include "include/coroutine.h"

#include "include/lib_coroutine.h"
#include <iostream>
#include "timer.h"


using lib::PHPCoroutine;
using lib::Coroutine;

lib_global_t LibG;

int init_poll()
{
    try{
        LibG.poll = new lib_poll_t();
    }catch(const std::exception& e){
        libError("%s",e.what());
    }
    LibG.poll->epollfd = epoll_create(256);
    if (LibG.poll->epollfd  < 0)
    {
        libWarn("Error has occurred: (errno %d) %s", errno, strerror(errno));
        free(LibG.poll);
        LibG.poll = NULL; // 新增的一行
        return -1;
    }


    LibG.poll->ncap = 16;
    try{
        LibG.poll->events = new epoll_event[LibG.poll->ncap]();
    }catch(const std::bad_alloc& e){
        libError("%s",e.what());
    }
    LibG.poll->event_num = 0;
    return OK;

}

int free_poll()
{
    if(close(LibG.poll->epollfd) < 0 ){
        libWarn("error has occurred (errno %d) %s",errno,strerror(errno));
    }
    delete[] LibG.poll->events;
    LibG.poll->events = nullptr;
    delete LibG.poll;
    LibG.poll = nullptr;
    return 0;
}

int event_init()
{
    if(!LibG.poll){
        init_poll();
    }
    LibG.running = 1;
    return 0;
}

int event_wait()
{
    event_init();

    while (LibG.running > 0)
    {
        int n;
        epoll_event *events;

        //获取最近的时间事件
        int timeout = search_nearest_time();
        events = LibG.poll->events;
        n = epoll_wait(LibG.poll->epollfd,LibG.poll->events,LibG.poll->ncap,timeout);
        for (int i = 0; i < n; i++) {
            int fd;
            int id;
            struct epoll_event *p = &events[i];
            uint64_t u64 = p->data.u64;
            Coroutine *co;

            fromuint64(u64, &fd, &id);
            co = Coroutine::get_by_cid(id);
            co->resume();
        }
        //处理定时任务触发事件
        process_time_event();

        if (!LibG.timeHead && LibG.poll->event_num == 0)
        {
            LibG.running = 0;
        }
    }
    event_free();
    return 0;
}


int event_free()
{
    LibG.running = 0;
    free_poll();
    return 0;
}