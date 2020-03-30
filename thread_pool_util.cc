#include "error.h"
#include "php_lib.h"
#include "thread_pool.h"
#include "lib_coroutine.h"
#include <memory>


typedef struct
{
    ThreadPool *pool;
    zend_object std;

}thread_pool;

ZEND_BEGIN_ARG_INFO_EX(arg_void, 0, 0, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arg_construct, 0, 0, 1)
ZEND_ARG_INFO(0, num)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arg_add,0,0,1)
ZEND_ARG_CALLABLE_INFO(0,func,0)
ZEND_END_ARG_INFO()
/**
 * Define zend class entry
 */
zend_class_entry thread_pool_ce;
zend_class_entry *thread_pool_ce_ptr;

static zend_object_handlers thread_pool_handlers;


static thread_pool* thread_pool_fetch_object(zend_object *obj)
{
    return (thread_pool *) ((char *) obj - thread_pool_handlers.offset);
}

static zend_object* thread_pool_create_object(zend_class_entry *ce)
{
    thread_pool *pool = (thread_pool *) ecalloc(1, sizeof(thread_pool) + zend_object_properties_size(ce));
    zend_object_std_init(&pool->std, ce);
    object_properties_init(&pool->std, ce);
    pool->std.handlers = &thread_pool_handlers;
    return &pool->std;
}

static void thread_pool_free_object(zend_object *object)
{
    thread_pool *pool = (thread_pool *) thread_pool_fetch_object(object);
    if (pool->pool)
    {
        delete pool->pool;
    }
    zend_object_std_dtor(&pool->std);
}

void php_lib_init_thread_pool_object(zval *zsocket, ThreadPool *pool)
{
    zend_object *object = thread_pool_create_object(thread_pool_ce_ptr);

    thread_pool *TPool = (thread_pool *) thread_pool_fetch_object(object);
    TPool->pool = pool;
    ZVAL_OBJ(zsocket, object);
}


static PHP_METHOD(thread_pool, __construct)
{
    zend_long num;
    thread_pool *pool;

    ZEND_PARSE_PARAMETERS_START(1, 1)
    Z_PARAM_LONG(num)
    ZEND_PARSE_PARAMETERS_END_EX(RETURN_FALSE);

    pool = (thread_pool *)thread_pool_fetch_object(Z_OBJ_P(getThis()));
    pool->pool = new ThreadPool(num);
}
PHP_METHOD(thread_pool,add)
{
    shared_ptr<php_lib_fci_fcc> call(new php_lib_fci_fcc());

    ZEND_PARSE_PARAMETERS_START(1,-1)
    Z_PARAM_FUNC(call->fci,call->fcc)
    Z_PARAM_VARIADIC("*",call->fci.params,call->fci.param_count)
    ZEND_PARSE_PARAMETERS_END_EX(RETURN_FALSE);

    thread_pool *pool = (thread_pool *)thread_pool_fetch_object(Z_OBJ_P(getThis()));

    pool->pool->enqueue([call]{
            zval result;
            call->fci.retval = &result;
            if (zend_call_function(&call->fci, &call->fcc) != SUCCESS)
            {
                php_error_docref(NULL, E_WARNING, "defer execute error");
                return;
            }
    });
}

// TODO: thread_pool get result
PHP_METHOD(thread_pool, get)
{

}
static const zend_function_entry thread_pool_methods[] =
        {
                PHP_ME(thread_pool, __construct, arg_construct, ZEND_ACC_PUBLIC | ZEND_ACC_CTOR)
                PHP_ME(thread_pool, add, arg_add, ZEND_ACC_PUBLIC)
                PHP_ME(thread_pool, get, arg_void, ZEND_ACC_PUBLIC)
                PHP_FE_END
        };

void lib_thread_pool_init(int module_number)
{
    INIT_NS_CLASS_ENTRY(thread_pool_ce, "Lib", "Thread\\Pool", thread_pool_methods);
    memcpy(&thread_pool_handlers, zend_get_std_object_handlers(), sizeof(zend_object_handlers));
    thread_pool_ce_ptr = zend_register_internal_class(&thread_pool_ce TSRMLS_CC); // Registered in the Zend Engine
    SET_CLASS_CUSTOM_OBJECT(thread_pool, thread_pool_create_object, thread_pool_free_object, thread_pool, std);
}