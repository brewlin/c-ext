#ifndef CONTEXT_H
#define CONTEXT_H

#include "asm_context.h"

typedef fcontext_t coroutine_context_t;
typedef void (*coroutine_func_t)(void*);

namespace Lib
{
    class Context
    {
    public:
        Context(size_t stack_size,coroutine_func_t fn,void *private_data)
    protected:
        coroutine_func_t fn_;
        uint32_t stack_size_;
        void *private_data_;

    };
}




#endif // !1CONTEXT