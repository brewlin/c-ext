#ifndef COROUTINE_H
#define COROUTINE_H
#include "context.h"

namespace Lib
{
    class coroutine
    {
        static long create(coroutine_func_t fn,void* args = nullptr);
    };
    
    
} // namespace name


#endif // !1CO COROUTINE_H