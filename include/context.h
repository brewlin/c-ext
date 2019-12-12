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
        Context(size_t stack_size,coroutine_func_t fn,void *private_data);
        static void context_func(void *arg);
        bool swap_out();
        bool swap_in();

        coroutine_context_t ctx;
        coroutine_context_t swap_ctx_;
        coroutine_func_t fn_;
        char* stack_;
        uint32_t stack_size_;
        void *private_data_;
        coroutine_context_t ctx_;

    };
}




#endif // !1CONTEXT