#include "lib.h"
#include "uv.h"
#include "log.h"
#include "coroutine.h"

#include "lib_coroutine.h"
#include <iostream>


using lib::PHPCoroutine;
using lib::Coroutine;

lib_global_t LibG;

int init_poll()
{
    size_t size;
//    LibG.poll = (lib_poll_t *)malloc(sizeof(lib_poll_t));
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
//    size = sizeof(struct epoll_event) * LibG.poll->ncap;
    //申请events 内存
//    LibG.poll->events = (struct epoll_event *)malloc(size);
    LibG.poll->event_num = 0;
//    memset(LibG.poll->events,0,size);

}

int free_poll()
{
    if(close(LibG.poll->epollfd) < 0 ){
        libWarn("error has occurred (errno %d) %s",errno,strerror(errno));
    }
    //free(LibG.poll->events);
    delete[] LibG.poll->events;
    LibG.poll->events = nullptr;
//    free(LibG.poll);
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
typedef enum
{
    UV_CLOCK_PRECISE = 0,  /* Use the highest resolution clock available. */
    UV_CLOCK_FAST = 1      /* Use the fastest clock with <= 1ms granularity. */
} uv_clocktype_t;
extern "C" void uv__run_timers(uv_loop_t* loop);
extern "C" uint64_t uv__hrtime(uv_clocktype_t type);
extern "C" int uv__next_timeout(const uv_loop_t* loop);
int event_wait()
{
    uv_loop_t *loop = uv_default_loop();
    event_init();


    while (LibG.running > 0)
    {
        int timeout; // 增加的代码
        int n;
        epoll_event *events;

        timeout = uv__next_timeout(loop); // 增加的代码
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
        // usleep(timeout); // 增加的代码

        loop->time = uv__hrtime(UV_CLOCK_FAST) / 1000000;
        uv__run_timers(loop);

        if (uv__next_timeout(loop) < 0 && LibG.poll->event_num == 0)
        {
            LibG.running = 0;
//            uv_stop(loop);
        }
    }
//    free_poll();
    event_free();
    return 0;
}


int event_free()
{
    LibG.running = 0;
    free_poll();
    return 0;
}