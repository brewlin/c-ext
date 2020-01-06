#ifndef LIB_COROUTINE_H
#define LIB_COROUTINE_H

#include "php_lib.h"
#include "coroutine.h"
#include <stack>

#define PHP_CORO_TASK_SLOT ((int)((ZEND_MM_ALIGNED_SIZE(sizeof(php_coro_task)) + ZEND_MM_ALIGNED_SIZE(sizeof(zval)) - 1) / ZEND_MM_ALIGNED_SIZE(sizeof(zval))))
#define DEFAULT_PHP_STACK_PAGE_SIZE 8192

//协程传递参数
struct php_coro_args
{
    zend_fcall_info_cache *fci_cache;
    zval *argv;
    uint32_t argc;
};
struct php_lib_fci_fcc
{
    zend_fcall_info fci;
    zend_fcall_info_cache fcc;
};

//保存协程栈
struct php_coro_task
{
    //协程栈顶
    zval *vm_stack_top;
    //协程栈栈底
    zval *vm_stack_end;
    //协程栈指针
    zend_vm_stack vm_stack;
    //协程栈页大小
    size_t vm_stack_page_size;
    //协程栈的栈帧
    zend_execute_data *execute_data;
    lib::Coroutine *co;
    std::stack<php_lib_fci_fcc *> *defer_tasks;

};


namespace lib
{
    class PHPCoroutine
    {
    public:
        static void defer(php_lib_fci_fcc *defer_fci_fcc);
        static long create(zend_fcall_info_cache *fci_cache,uint32_t argc,zval *argv);
        static int sleep(double seconds);
        static int scheduler();

        static void init();
        static inline php_coro_task* get_origin_task(php_coro_task *task)
        {
            Coroutine *co = task->co->get_origin();
            return co?(php_coro_task *)co->get_task():&main_task;
        }

    protected:
        //主协程成员
        static php_coro_task main_task;

        static void save_task(php_coro_task *task);
        //保存协程栈内容
        static void save_vm_stack(php_coro_task *task);
        //获取当前协程栈信息
        static php_coro_task* get_task();
        static void create_func(void *arg);
        //初始化一个php栈
        static void vm_stack_init(void);

        static void on_yield(void *arg);
        static void on_resume(void *arg);
        static void on_close(void *arg);
        static inline void restore_task(php_coro_task *task);
        static inline void restore_vm_stack(php_coro_task *task);
    };
}
//????????php?????
ZEND_BEGIN_ARG_INFO_EX(arginfo_lib_coroutine_create,0,0,1)
ZEND_ARG_CALLABLE_INFO(0,func,0)
ZEND_END_ARG_INFO()





PHP_FUNCTION(lib_coroutine_create);
PHP_FUNCTION(event_init);
PHP_FUNCTION(event_wait);


#endif // !1LIB_COROUTINE_#define LIB_COROUTINE_HH
