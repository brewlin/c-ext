#include "coroutine.h"
#include "timer.h"

using lib::Coroutine;

size_t Coroutine::stack_size = DEFAULT_C_STACK_SIZE;
Coroutine* Coroutine::current  = nullptr;
std::unordered_map<long, Coroutine*> Coroutine::coroutines;
long Coroutine::last_cid = 0;

lib_coro_on_swap_t Coroutine::on_yield = nullptr;
lib_coro_on_swap_t Coroutine::on_resume = nullptr;

void Coroutine::set_on_yield(lib_coro_on_swap_t func)
{
    on_yield = func;
}

void Coroutine::set_on_resume(lib_coro_on_swap_t func)
{
    on_resume = func;
}

void * Coroutine::get_current_task()
{
    return Coroutine::current ? Coroutine::current->get_task():nullptr;
}

void *Coroutine::get_task()
{
    return task;
}
void Coroutine::set_task(void *_task)
{
    task = _task;
}

long Coroutine::create(coroutine_func_t fn,void *args)
{
    return (new Coroutine(fn,args))->run();
}
Coroutine* Coroutine::get_current()
{
    return current;
}
void Coroutine::yield()
{
    current = origin;
    ctx.swap_out();
}
void Coroutine::resume()
{
    origin = current;
    current = this;
    ctx.swap_in();
    if (ctx.is_end())
    {
        cid = current->get_cid();
        printf("in resume method: co[%ld] end\n", cid);
        current = origin;
        coroutines.erase(cid);
        delete this;
    }
}
int sleep_call(long long id,void *data){
    ((Coroutine *)data)->resume();
    return NOMORE;
}
int Coroutine::sleep(double seconds)
{
    Coroutine* co = Coroutine::get_current();
    create_time_event(seconds,sleep_call,co,NULL);
    co->yield();
    return 0;

}