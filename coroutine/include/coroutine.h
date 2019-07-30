#ifndef COROUTINE_H
#define COROUTINE_H

#include "context.h"

namespace Lib
{
    class Coroutine
    {
        static long create(coroutine_func_t fn,void * args = nullptr);
    };
}

#endif // !1COROUTINE_H