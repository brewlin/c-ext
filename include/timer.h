//
// Created by Administrator on 2019/12/24 0024.
//

#ifndef C_EXT_TIMER_H
#define C_EXT_TIMER_H

#include "php_lib.h"
#include "lib.h"

typedef int TimeProc(long long id,void *data);
typedef void FinalizerProc(void *data);

//非周期性定时任务
#define  NOMORE -1

typedef struct TimeEvent {
    //时间事件id
    long long id;

    //事件到达时间 seconds
    long when_sec;
    long when_ms; //milliseconds

    //事件处理函数
    TimeProc *proc;

    //事件释放函数
    FinalizerProc *fproc;

    void *data;

//    指向下一个时间结构，形成链表
    struct TimeEvent *next;
}TimeEvent;

/*
定时时间到
*/
int time_expire(long long id, void *clientData);

//创建时间事件
long long create_time_event(long long millseconds,TimeProc *proc,void *data,FinalizerProc *finalizerProc);
//设置处理事件的时间
void add_milliseconds_tonow(long long milliseconds,long *sec,long *ms);
/*
 * 取出当前时间的秒和毫秒，
 * 并分别将它们保存到 seconds 和 milliseconds 参数中
 */
void get_time(long *seconds, long *milliseconds);
/**
 * 处理已到达时间事件
 * @return
 */
int process_time_event();
/**
 * 搜寻最近的时间事件
 * @return
 */
TimeEvent *search_nearest_time();
/*
 * 删除给定 id 的时间事件
 */
int del_time(long long id)

void init()
{
    create_time_event(1,time_expire,NULL,NULL);
}

void main()
{

    LibG.running = 1;

    while(LibG.running){
        //预处理

        int processed = 0,numevents;

        int j;
        TimeEvent *shorttest  = NULL;
        struct timeval tv,*tvp;

        //获取最近的时间事件
        shorttest = search_nearest_time();

        if(shorttest){
            // 如果时间事件存在的话
            // 那么根据最近可执行时间事件和现在时间的时间差来决定文件事件的阻塞时间
            long now_sec,now_ms;

            // 计算距今最近的时间事件还要多久才能达到
            // 并将该时间距保存在 tv 结构
            get_time(&now_sec,&now_ms);
            tvp = &tv;
            tvp->tv_sec = shorttest->when_sec - now_sec;
            if (shortest->when_ms < now_ms) {
                tvp->tv_usec = ((shortest->when_ms+1000) - now_ms)*1000;
                tvp->tv_sec --;
            } else {
                tvp->tv_usec = (shortest->when_ms - now_ms)*1000;
            }
            // 时间差小于 0 ，说明事件已经可以执行了，将秒和毫秒设为 0 （不阻塞）
            if (tvp->tv_sec < 0) tvp->tv_sec = 0;
            if (tvp->tv_usec < 0) tvp->tv_usec = 0;

        }

        //执行event wait  获取触发事件
//        numevents = event_wait(tvp);

        //执行时间事件
        processed += process_time_event();

    }
}

PHP_FUNCTION(lib_timer_test);

#endif //C_EXT_TIMER_H
