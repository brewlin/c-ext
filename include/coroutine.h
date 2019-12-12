#ifndef COROUTINE_H
#define COROUTINE_H

#include "context.h"
#include <unordered_map>

#define DEFAULT_C_STACK_SIZE          (2 *1024 * 1024)
namespace Lib
{
    class Coroutine
    {
    public:
        static std::unordered_map<long, Coroutine*> coroutines;
        //获取当前协程
        static void* get_current_task();
        static Coroutine* get_current();
        static long create(coroutine_func_t fn,void * args = nullptr);
        void* get_task();
        void set_task(void *_task);

    
    protected:
        static Coroutine* current;
        Coroutine *origin;
        void *task = nullptr;
        Context ctx;
        static size_t stack_size;
        long cid;
        static long last_cid;

        Coroutine(coroutine_func_t fn,void *private_data):
        ctx(stack_size,fn,private_data)
        {
            cid = ++last_cid;
            coroutines[cid] = this;
        }
        long run()
        {
            long cid = this->cid;
            origin = current;
            current = this;
            ctx.swap_in();
            return cid;
        }
    };
}

#endif // !1COROUTINE_H