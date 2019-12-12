#include "coroutine.h"

using Lib::Coroutine;
size_t Coroutine::stack_size = DEFAULT_C_STACK_SIZE;
Coroutine* Courutine::current = nullptr;
long Coroutine::last_cid = 0;

void * Courutine::get_currrent_task()
{
    return Courutine::current ? Courutine::current->get_task():nullptr;
}

void *Coroutine::get_task()
{
    return task;
}