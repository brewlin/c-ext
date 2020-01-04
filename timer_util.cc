#include "php_lib.h"
#include "lib_coroutine.h"
#include "timer.h"

zend_class_entry lib_timer_ce;
zend_class_entry *lib_timer_ce_ptr;

ZEND_BEGIN_ARG_INFO_EX(arginfo_lib_timer_arg, 0, 0, 2)
ZEND_ARG_INFO(0, seconds)
ZEND_ARG_CALLABLE_INFO(0, func, 0)
ZEND_END_ARG_INFO()

int tick(long long id,void *data){

    php_lib_fci_fcc *fci = (php_lib_fci_fcc *)data;
    zval result;
    fci->fci.retval = &result;
    if(zend_call_function(&fci->fci,&fci->fcc) != SUCCESS){
        return NOMORE;
    }
    return NOMORE;
}
PHP_METHOD(timer_obj,tick)
{
    php_lib_fci_fcc *fci = (php_lib_fci_fcc *)malloc(sizeof(php_lib_fci_fcc));

    zend_long seconds = 0;
    //强制传入两个参数
    ZEND_PARSE_PARAMETERS_START(2, 2)
    Z_PARAM_LONG(seconds)
    Z_PARAM_FUNC(fci->fci,fci->fcc)
    ZEND_PARSE_PARAMETERS_END_EX(RETURN_FALSE);

    long id = create_time_event(seconds,tick,fci,NULL);
    RETURN_LONG(id);
}

PHP_METHOD(timer_obj,after)
{
    //????
    zend_fcall_info fci = empty_fcall_info;
    zend_fcall_info_cache fcc = empty_fcall_info_cache;
    zval result;
    zend_long seconds = 0;
    //1 -1 ????????? ??????
    ZEND_PARSE_PARAMETERS_START(1,-1)
    Z_PARAM_LONG(seconds)
    Z_PARAM_FUNC(fci,fcc)
//    Z_PARAM_VARIADIC("*",fci.params,fci.param_count)
    ZEND_PARSE_PARAMETERS_END_EX(RETURN_FALSE);

    fci.retval = &result;
    if(zend_call_function(&fci,&fcc) != SUCCESS){
    return;
    }
    *return_value = result;
}

const zend_function_entry lib_timer_util_methods[] =
        {
                PHP_ME(timer_obj,tick,arginfo_lib_timer_arg,ZEND_ACC_PUBLIC | ZEND_ACC_STATIC)
                PHP_ME(timer_obj,after,arginfo_lib_timer_arg,ZEND_ACC_PUBLIC | ZEND_ACC_STATIC)
                PHP_FE_END
        };


void lib_timer_util_init(){
    INIT_NS_CLASS_ENTRY(lib_timer_ce,"Lib","Timer",lib_timer_util_methods);
    lib_timer_ce_ptr = zend_register_internal_class(&lib_timer_ce TSRMLS_CC);
}