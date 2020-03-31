#include "thread_pool.h"

ThreadPool::ThreadPool(size_t threads)
:stop(false)
{
    cout << "start " << threads << " threads" << endl;
    for(size_t i = 0; i < threads ; i ++){
        workers.emplace_back([this]{
            for(;;){
                function<void()> task;
                {
                    unique_lock<mutex> lock(this->queue_mu);
                    this->cond.wait(lock,[this]{
                        return this->stop || !this->tasks.empty();
                    });

                    if(this->stop && this->tasks.empty())
                        return;

                    task = move(this->tasks.front());
                    this->tasks.pop();
                }
                task();
            }
        });
    }

}

ThreadPool::~ThreadPool(){
    {
        unique_lock<mutex> lock(queue_mu);
        stop = true;
    }
    cond.notify_all();
    for(thread &w : workers){
        w.join();
    }

}

