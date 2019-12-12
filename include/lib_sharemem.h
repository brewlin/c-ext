#ifndef LIB_COROUTINE_H
#define LIB_COROUTINE_H
#define DEFAULT_PHP_STACK_PAGE_SIZE 8192
#include "php_lib.h"
#include <sys/mman.h>

namespace Lib
{
    class ShareMem
    {
    public:
        static long create(zend_fcall_info_cache *fci_cache,uint32_t argc,zval *argv);
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
    };
}


#endif // !1LIB_COROUTINE_#define LIB_COROUTINE_HH
