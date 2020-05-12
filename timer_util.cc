#include "php_lib.h"
#include "timer.h"
#include "zend_callback.h"

zend_class_entry lib_timer_ce;
zend_class_entry *lib_timer_ce_ptr;

ZEND_BEGIN_ARG_INFO_EX(timer_arg, 0, 0, 2)
ZEND_ARG_INFO(0, seconds)
ZEND_ARG_CALLABLE_INFO(0, func, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(timer_del, 0, 0, 1)
ZEND_ARG_INFO(0, timerid)
ZEND_END_ARG_INFO()

/**
 * del callback param
 **/
static void del(void *data)
{
    CallBackParam *call = (CallBackParam *)data;
    delete call;
}

/**
 * tick
 **/
int tick(long long id,void *data){

    CallBackParam *call = (CallBackParam *)data;
    zval result;
    call->call(&result);
    return call->seconds;
}
PHP_METHOD(timer_obj,tick)
{
    CallBackParam *call = new CallBackParam;
    //强制传入两个参数
    ZEND_PARSE_PARAMETERS_START(2, 2)
    Z_PARAM_LONG(call->seconds)
    Z_PARAM_FUNC(call->fci,call->fcc)
    ZEND_PARSE_PARAMETERS_END_EX(RETURN_FALSE);
    //add reference count
    call->AddRefCount();
    long id = create_time_event(call->seconds,tick,call,del);
    RETURN_LONG(id);
}
int after(long long id,void *data){

    CallBackParam *call = (CallBackParam *)data;
    zval result;
    call->call(&result);
    return NOMORE;
}
PHP_METHOD(timer_obj,after)
{
    CallBackParam *call = new CallBackParam;
    //强制传入两个参数
    ZEND_PARSE_PARAMETERS_START(2, 2)
    Z_PARAM_LONG(call->seconds)
    Z_PARAM_FUNC(call->fci,call->fcc)
    ZEND_PARSE_PARAMETERS_END_EX(RETURN_FALSE);
    //add reference count
    call->AddRefCount();
    long id = create_time_event(call->seconds,after,call,del);
    RETURN_LONG(id)
}
PHP_METHOD(timer_obj,del)
{
    long timer_id;
    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "l", &timer_id) == FAILURE)
    {
        return;
    }
    int ret = del_time(timer_id);
    if(ret == OK){
        RETURN_TRUE;
    }
    RETURN_FALSE;

}
const zend_function_entry lib_timer_util_methods[] =
{
    PHP_ME(timer_obj,tick,timer_arg,ZEND_ACC_PUBLIC | ZEND_ACC_STATIC)
    PHP_ME(timer_obj,after,timer_arg,ZEND_ACC_PUBLIC | ZEND_ACC_STATIC)
    PHP_ME(timer_obj,del,timer_del,ZEND_ACC_PUBLIC | ZEND_ACC_STATIC)
    PHP_FE_END
};


void lib_timer_util_init(){
    INIT_NS_CLASS_ENTRY(lib_timer_ce,"Lib","Timer",lib_timer_util_methods);
    lib_timer_ce_ptr = zend_register_internal_class(&lib_timer_ce TSRMLS_CC);
}