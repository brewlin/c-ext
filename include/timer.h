//
// Created by Administrator on 2019/12/24 0024.
//

#ifndef C_EXT_TIMER_H
#define C_EXT_TIMER_H

#include "lib.h"



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
int del_time(long long id);


#endif //C_EXT_TIMER_H
