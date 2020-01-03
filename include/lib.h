#ifndef LIB_H_
#define LIB_H_

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

// include standard library
#include <assert.h>
#include <ctype.h>
#include <errno.h>
#include <inttypes.h>
#include <limits.h>
#include <math.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <signal.h>
#include <time.h>

#include <fcntl.h>
#include <unistd.h>
#include <pthread.h>
#include <poll.h>

#include <arpa/inet.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <netdb.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/select.h>
#include <sys/mman.h>
#include <sys/ipc.h>
#include <sys/wait.h>
#include <sys/un.h>
#include <sys/types.h>
#include <sys/utsname.h>
#include <sys/stat.h>
//引入epoll
#include <sys/epoll.h>

#include "timer.h"

typedef unsigned int uint_t;
typedef int int_t;

typedef int pid_t;
typedef int fd_t;

typedef int  socket_t;
typedef int  err_t;

#define ERROR -1
#define OK 0
#define AGAIN 1

#define memzero(buf, n)       (void) memset(buf, 0, n)

#define ncpu 3

//#define value_helper(n)   ##n
//#define value(n) value_helper(n)

#define signal_helper(n)     SIG##n
#define signal_value(n)      signal_helper(n)

enum libevent_type
{
    LIB_EVENT_NULL   = 0,
    LIB_EVENT_DEAULT = 1u << 8,
    LIB_EVENT_READ   = 1u << 9,
    LIB_EVENT_WRITE  = 1u << 10,
    LIB_EVENT_RDWR   = LIB_EVENT_READ | LIB_EVENT_WRITE,
    LiB_EVENT_ERROR  = 1u << 11,
};

typedef struct
{
    //epollfd  => epoll_create
    int epollfd;
    //events数组大小
    int ncap;
    //事件计数器
    int event_num;
    //触发的events事件
    struct epoll_event *events;
}lib_poll_t;

typedef struct
{
    //定义指针类型，方便判断是否初始化申请过内存if(!poll)
    lib_poll_t *poll;
    //循环是否结束
    int running;

    //时间事件id
    long long timeNextId;
    //最后一次执行时间
    time_t lastTime;
    //时间事件链表
    TimeEvent *timeHead;
}lib_global_t;
//申明在其他地方定义的全局变量
extern lib_global_t LibG;

static inline uint64_t touint64(int fd, int id)
{
    uint64_t ret = 0;
    ret |= ((uint64_t)fd) << 32;
    ret |= ((uint64_t)id);

    return ret;
}
static inline void fromuint64(uint64_t v, int *fd, int *id)
{
    *fd = (int)(v >> 32);
    *id = (int)(v & 0xffffffff);
}

int init_poll();
int free_poll();

int event_init();
int event_wait();
int event_free();



#endif /* LIB_H_ */