#include "timer.h"

/*
 * 取出当前时间的秒和毫秒，
 * 并分别将它们保存到 seconds 和 milliseconds 参数中
 */
void get_time(long *seconds, long *milliseconds)
{
    struct timeval tv;

    gettimeofday(&tv, NULL);
    *seconds = tv.tv_sec;
    *milliseconds = tv.tv_usec/1000;
}
/**
 *
 * 在当前时间上加上milliseconds 毫秒
 * 并将加上之后的秒数和毫秒数分别保存在 sec 和 ms 指针上
 * @param milliseconds
 * @param sec
 * @param ms
 */
void add_milliseconds_tonow(long long milliseconds,long *sec,long *ms){

    long cur_sec,cur_ms,when_sec,when_ms;

    get_time(&cur_sec,&cur_ms);

    //计算增加milliseconds 之后的秒速和毫秒数
    when_sec = cur_sec + milliseconds/1000;
    when_ms = cur_ms +milliseconds%1000;

    // 进位：
    // 如果 when_ms 大于等于 1000
    // 那么将 when_sec 增大一秒
    if (when_ms >= 1000) {
        when_sec ++;
        when_ms -= 1000;
    }

    // 保存到指针中
    *sec = when_sec;
    *ms = when_ms;
}
/**
 * 创建时间结构
 * @param millseconds
 * @param proc
 * @param data
 * @param finalizerProc
 * @return
 */
long long create_time_event(long long millseconds,TimeProc *proc,void *data,FinalizerProc *finalizerProc)
{
    //更新计数器
    long long id = LibG.timeNextId++;

    TimeEvent *te;
    te = (TimeEvent *)malloc(sizeof(*te));
    if(te == NULL)return ERROR;

    te->id = id;

    //设置处理事件的时间
    add_milliseconds_tonow(millseconds,&te->when_sec,&te->when_ms);
    //设置事件处理
    te->proc = proc;
    te->fproc = finalizerProc;
    //私有数据
    te->data = data;

    //将当前事件插入表头
    te->next = LibG.timeHead;
    LibG.timeHead = te;

    return id;
}


/*
定时时间到
*/
int time_expire(long long id, void *clientData)
{
    printf("MainTimerExpire\n");

    //15秒后再次执行该函数,如果这里返回0，则定时器只会执行一次，返回大于0，则是周期性定时器
    return 15000;
}
/**
 * 获取最近一次时间
 * @return
 */
int search_nearest_time()
{
    struct timeval tv,*tvp;
    tvp = &tv;
    TimeEvent *te = LibG.timeHead;
    //最近的时间事件
    TimeEvent *nearest = NULL;

    while(te) {
        if (!nearest || te->when_sec < nearest->when_sec ||(te->when_sec == nearest->when_sec && te->when_ms < nearest->when_ms))
            nearest = te;

        te = te->next;
    }

    if(nearest){
        long now_sec,now_ms;
        get_time(&now_sec,&now_ms);
        tvp->tv_sec = nearest->when_sec - now_sec;
        if (nearest->when_ms < now_ms) {
            tvp->tv_usec = ((nearest->when_ms+1000) - now_ms)*1000;
            tvp->tv_sec --;
        } else {
            tvp->tv_usec = (nearest->when_ms - now_ms)*1000;
        }
        // 时间差小于 0 ，说明事件已经可以执行了，将秒和毫秒设为 0 （不阻塞）
        if (tvp->tv_sec < 0) tvp->tv_sec = 0;
        if (tvp->tv_usec < 0) tvp->tv_usec = 0;
    }
    return tvp ? (tvp->tv_sec*1000 + tvp->tv_usec/1000) : -1;
}

/**
 * 处理已到达时间事件
 * @return
 */
int process_time_event()
{
    int processed = 0;
    TimeEvent *te;
    long long maxId;
    time_t now = time(NULL);

    // 通过重置事件的运行时间，
    // 防止因时间穿插（skew）而造成的事件处理混乱
    if (now < LibG.lastTime) {
        te = LibG.timeHead;
        while(te) {
            te->when_sec = 0;
            te = te->next;
        }
    }
    // 更新最后一次处理时间事件的时间
    LibG.lastTime = now;

    // 遍历链表
    // 执行那些已经到达的事件
    te = LibG.timeHead;
    maxId = LibG.timeNextId - 1;
    while(te) {
        long now_sec, now_ms;
        long long id;

        // 跳过无效事件
        if (te->id > maxId) {
            te = te->next;
            continue;
        }

        // 获取当前时间
        get_time(&now_sec, &now_ms);

        // 如果当前时间等于或等于事件的执行时间，那么说明事件已到达，执行这个事件
        if (now_sec > te->when_sec ||
            (now_sec == te->when_sec && now_ms >= te->when_ms))
        {
            int retval;

            id = te->id;
            // 执行事件处理器，并获取返回值
            retval = te->proc( id, te->data);
            processed++;
            /**
                如果事件处理器返回NOMORE，那么这个事件为定时事件：该事件在达到一次之后就会被删除，之后不再到达。
                如果事件处理器返回一个非AE NOMORE的整数值，那么这个事件为周期性时间：当一个时间事件到达之后，服务器会根据事件处理器返回的值，
                对时间事件的when属性进行更新，让这个事件在一段时间之后再次到达，并以这种方式一直更新并运行下去。
                比如说，如果一个时间事件的赴理器返回整数值30，那么服务器应该对这个时间事件进行更新，让这个事件在30毫秒之后再次到达。
             *  决定时间事件是否要持续执行的 flag
             **/
            // 记录是否有需要循环执行这个事件时间
            if (retval != NOMORE) {
                // 是的， retval 毫秒之后继续执行这个时间事件
                add_milliseconds_tonow(retval,&te->when_sec,&te->when_ms);
            } else {
                // 不，将这个事件删除
                del_time(id);
            }

            // 因为执行事件之后，事件列表可能已经被改变了
            // 因此需要将 te 放回表头，继续开始执行事件
            te = LibG.timeHead;
        } else {
            te = te->next;
        }
    }
    return processed;
}

/*
 * 删除给定 id 的时间事件
 */
int del_time(long long id)
{
    TimeEvent *te, *prev = NULL;

    // 遍历链表
    te = LibG.timeHead;
    while(te) {

        // 发现目标事件，删除
        if (te->id == id) {

            if (prev == NULL)
                LibG.timeHead = te->next;
            else
                prev->next = te->next;

            // 执行清理处理器
            if (te->fproc)
                te->fproc(te->data);

            // 释放时间事件
            free(te);
            return OK;
        }
        prev = te;
        te = te->next;
    }

    return ERROR; /* NO event with the specified ID found */
}


//
//uint64_t repeat = 0;
//
//static void callback(uv_timer_t *handle)
//{
//    repeat = repeat + 1;
//    printf("repeat count:%d \n", repeat);
//}
//
//PHP_FUNCTION(lib_timer_test)
//        {
//                uv_loop_t *loop = uv_default_loop();
//                uv_timer_t timer_req;
//                uv_timer_init(loop, &timer_req);
//
//                uv_timer_start(&timer_req, callback, 1000, 1000);
//                uv_run(loop, UV_RUN_DEFAULT);
//        }
