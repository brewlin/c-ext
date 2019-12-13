#include "coroutine.h"

using Lib::Coroutine;
size_t Coroutine::stack_size = DEFAULT_C_STACK_SIZE;
Coroutine* Coroutine::current  = nullptr;
std::unordered_map<long, Coroutine*> Coroutine::coroutines;
long Coroutine::last_cid = 0;


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