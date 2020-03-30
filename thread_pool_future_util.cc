#include "error.h"
#include "php_lib.h"

#include <future>

using namespace std;

struct thread_pool_future
{
    future<string> fu;
    zend_object std;

};
ZEND_BEGIN_ARG_INFO_EX(arg_void, 0, 0, 0)
ZEND_END_ARG_INFO()

/**
 * Define zend class entry
 */
zend_class_entry thread_pool_future_ce;
zend_class_entry *thread_pool_future_ce_ptr;

static zend_object_handlers thread_pool_future_handlers;


static thread_pool_future* thread_pool_future_fetch_object(zend_object *obj)
{
    return (thread_pool_future *) ((char *) obj - thread_pool_future_handlers.offset);
}

static zend_object* thread_pool_future_create_object(zend_class_entry *ce)
{
    thread_pool_future *pool = (thread_pool_future *) ecalloc(1, sizeof(thread_pool_future) + zend_object_properties_size(ce));
    zend_object_std_init(&pool->std, ce);
    object_properties_init(&pool->std, ce);
    pool->std.handlers = &thread_pool_future_handlers;
    return &pool->std;
}

static void thread_pool_future_free_object(zend_object *object)
{
    thread_pool_future *pool = (thread_pool_future *) thread_pool_future_fetch_object(object);
    // if (pool->fu)
    // {
        // delete pool->pool;
    // }
    zend_object_std_dtor(&pool->std);
}

void php_lib_init_thread_pool_future_object(zval *zsocket, future<string> fu)
{
    zend_object *object = thread_pool_future_create_object(thread_pool_future_ce_ptr);

    thread_pool_future *fut = (thread_pool_future *) thread_pool_future_fetch_object(object);
    fut->fu = fu;
    ZVAL_OBJ(zsocket, object);
}


static PHP_METHOD(thread_pool_future, __construct)
{
}


static PHP_METHOD(thread_pool_future, get)
{
    auto fu = (thread_pool_future *)thread_pool_future_fetch_object(Z_OBJ_P(getThis()));
    RETURN_STRING(fu.get());
}
static const zend_function_entry thread_pool_future_methods[] =
        {
                PHP_ME(thread_pool_future, __construct, arg_void, ZEND_ACC_PUBLIC | ZEND_ACC_CTOR)
                PHP_ME(thread_pool_future, get, arg_void, ZEND_ACC_PUBLIC)
                PHP_FE_END
        };

void lib_thread_pool_future_init(int module_number)
{
    INIT_NS_CLASS_ENTRY(thread_pool_future_ce, "Lib", "Thread\\Pool\Future", thread_pool_future_methods);
    memcpy(&thread_pool_future_handlers, zend_get_std_object_handlers(), sizeof(zend_object_handlers));
    thread_pool_future_ce_ptr = zend_register_internal_class(&thread_pool_future_ce TSRMLS_CC); // Registered in the Zend Engine
    SET_CLASS_CUSTOM_OBJECT(thread_pool_future, thread_pool_future_create_object, thread_pool_future_free_object, thread_pool_future, std);
}