#include "error.h"
#include "php_lib.h"
#include "lib_coroutine.h"
#include <memory>
#include "zend_callback.h"
#include "lib_class.h"
#include <future>
#include "thread_pool_future.h"

ZEND_BEGIN_ARG_INFO_EX(arg_void, 0, 0, 0)
ZEND_END_ARG_INFO()

zend_class_entry thread_pool_future_ce;
zend_class_entry *thread_pool_future_ce_ptr;
zend_object_handlers thread_pool_future_handlers;

PHP_METHOD(thread_pool, wait)
{
    auto obj = thread_pool_future::fetch_object(Z_OBJ_P(getThis()));
    obj->fu.get();
}
static const zend_function_entry thread_pool_future_methods[] =
    {
        PHP_ME(thread_pool, wait, arg_void, ZEND_ACC_PUBLIC)
        PHP_FE_END
    };

void lib_thread_pool_future_init(int module_number)
{
    INIT_NS_CLASS_ENTRY(thread_pool_future_ce, "Lib", "Thread\\Pool\\Future", thread_pool_future_methods);
    memcpy(&thread_pool_future_handlers, zend_get_std_object_handlers(), sizeof(zend_object_handlers));
    thread_pool_future_ce_ptr = zend_register_internal_class(&thread_pool_future_ce TSRMLS_CC); // Registered in the Zend Engine

    //注册绑定自定义类到php类
    REGISTER_CUSTOM_OBJECT(thread_pool_future);
    // thread_pool_ce_ptr->create_object = thread_pool::create_object;
    // thread_pool_handlers.free_obj = thread_pool::free_object;
    // thread_pool_handlers.offset = __builtin_offsetof (thread_pool, std);
}