#ifndef __THREADPOOL__H__
#define __THREADPOOL__H__
#include <future>
#include <vector>
#include <functional>
#include <queue>
#include <thread>
#include <iostream>

using namespace std;


class ThreadPool {
public:
    ThreadPool(size_t threads);

    template<typename F,typename... Args>
    auto enqueue(F && f,Args&&... args)->future<decltype(f(args...))>;

    ~ThreadPool(); 
private:
    vector<thread> workers;

    queue<function<void()>> tasks;

    mutex queue_mu;
    condition_variable cond;

    bool stop;

};


template<typename F,typename... Args>
auto ThreadPool::enqueue(F&& f,Args&&... args) -> future<decltype(f(args...))>{

    using ret_type = decltype(f(args...));

    auto task = make_shared<packaged_task<ret_type()>>(bind(forward<F>(f),forward<Args>(args)...));

    auto res = task->get_future();

    {
        unique_lock<mutex> lock(queue_mu);
        if(stop)
            throw runtime_error("thread pool is stop");
        this->tasks.emplace([task]{
            (*task)();
        });
    }
    cond.notify_one();
    return res;
}



#endif  //!__THREADPOOL__H__