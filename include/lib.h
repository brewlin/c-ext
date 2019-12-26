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



#define value_helper(n)   #n
#define value(n)          value_helper(n)

#define signal_helper(n)     SIG##n
#define signal_value(n)      signal_helper(n)

typedef struct
{
    //epollfd  => epoll_create
    int epollfd;
    //events数组大小
    int ncap;
    //触发的events事件
    struct epoll_event *events;
}lib_poll_t;

typedef struct
{
    lib_poll_t poll;
}lib_global_t;
//申明在其他地方定义的全局变量
extern lib_global_t LibG;
#endif /* LIB_H_ */