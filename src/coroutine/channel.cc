#include "include/coroutine_channel.h"
#include "include/coroutine.h"
#include "include/timer.h"
#include <queue>

using lib::Coroutine;
using lib::coroutine::Channel;

Channel::Channel(size_t _capacity) {

}
Channel::~Channel() {

}
//协程超时后 执行的函数
static int sleep_timeout(long long id,void *param)
{
    ((Coroutine *)param)->resume();
    return NOMORE;
}

//消费者消费 pop
void *Channel::pop(double timeout)
{
    Coroutine *co = Coroutine::get_current();
    void *data;
    //为空则切换协程 等待超时后再唤起协程
    if(data_queue.empty()){
        if(timeout > 0 ){
            create_time_event(timeout,sleep_timeout,(void *)co,NULL);
        }
        //加入消费者队列
        consumer_queue.push(co);
        co->yield();
    }

    //超时唤起协程后依然为空 则返回错误
    if(data_queue.empty()){
        return nullptr;
    }

    //队列里有数据可以消费
    data = data_queue.front();
    data_queue.pop();

    //如果有生产者在等待，就唤起生产者
    if(!producer_queue.empty()){
        //唤起生产者
        co = producer_queue.front();
        producer_queue.pop();
        //唤起生产者
        co->resume();
    }
    return data;
}

bool Channel::push(void *data, double timeout)
{
    Coroutine *co = Coroutine::get_current();

    //如果通道队列满了的情况下需要等待释放空间
    if(data_queue.size() == capacity){
        if(timeout > 0 ){
            create_time_event(timeout,sleep_timeout,(void *)co,NULL);
        }
        producer_queue.push(co);
        co->yield();
    }

    if(data_queue.size() == capacity){return false;}

    data_queue.push(data);

    //如果有生产者在等待，则唤起该生产者
    if(!consumer_queue.empty()){
        co = consumer_queue.front();
        consumer_queue.pop();
        co->resume();
    }
    return true;
}
bool Channel::empty()
{
    return data_queue.empty();
}

void* Channel::pop_data()
{
    void *data;

    if (data_queue.size() == 0)
    {
        return nullptr;
    }
    data = data_queue.front();
    data_queue.pop();
    return data;
}