#include "coroutine.h"

using Lib::Coroutine;

Coroutine* Courutine::current = nullptr;

void * Courutine::get_currrent_task()
{
    return Courutine::current ? Courutine::current->get_task():nullptr;
}

void *Coroutine::get_task()
{
    return task;
}