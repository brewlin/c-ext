#ifndef COROUTINE_H
#define COROUTINE_H

#include "context.h"
#include <unordered_map>

#define DEFAULT_C_STACK_SIZE          (2 *1024 * 1024)

typedef void (*lib_coro_on_swap_t)(void*);

namespace lib
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
        //内联
        inline long get_cid()
        {
            return cid;
        }
        void yield();
        void resume();
        static int sleep(double seconds);
        static inline Coroutine* get_by_cid(long cid)
        {
            auto i = coroutines.find(cid);
            return i != coroutines.end() ? i->second: nullptr;
        }

        static void set_on_yield(lib_coro_on_swap_t func);
        static void set_on_resume(lib_coro_on_swap_t func);

        inline Coroutine* get_origin()
        {
            return origin;
        }

    
    protected:
        static lib_coro_on_swap_t on_yield;
        static lib_coro_on_swap_t on_resume;

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
            //加载携程上下文
            ctx.swap_in();
            if(ctx.is_end())
            {
                cid = current->get_cid();
                printf("in run method: co[%ld] end\n", cid);
                current = origin;
                coroutines.erase(cid);
                delete this;
            }
            return cid;
        }
    };
}

#endif // !1COROUTINE_H