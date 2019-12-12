#include "lib_coroutine.h"

using Lib::PHPCoroutine;

ZEND_BEGIN_ARG_INFO_EX(arginfo_lib_coroutine_create,0,0,1)
ZEND_ARG_CALLABLE_INFO(0,func,0)
ZEND_END_ARG_INFO()

//声明方法
static PHP_METHOD(lib_coroutine_util,create);


PHP_METHOD(lib_coroutine_util,create)
{
    //????
    zend_fcall_info fci = empty_fcall_info;
    zend_fcall_info_cache fcc = empty_fcall_info_cache;
    zval result;
    //1 -1 ????????? ??????
    ZEND_PARSE_PARAMETERS_START(1,-1)
        Z_PARAM_FUNC(fci,fcc)
        Z_PARAM_VARIADIC("*",fci.params,fci.param_count)
    ZEND_PARSE_PARAMETERS_END_EX(RETURN_FALSE);
    PHPCoroutine::create(&fcc,fci.param_count,fci.params);
    // fci.retval = &result;
    // if(zend_call_function(&fci,&fcc) != SUCCESS){
        // return;
    // }
    // *return_value = result;
}

const zend_function_entry lib_coroutine_util_methods[] = 
{
    PHP_ME(lib_coroutine_util,create,arginfo_lib_coroutine_create,ZEND_ACC_PUBLIC | ZEND_ACC_STATIC)
    PHP_FE_END
};

zend_class_entry lib_coroutine_ce;
zend_class_entry *lib_coroutine_ce_ptr;
void lib_coroutine_util_init()
{
    INIT_NS_CLASS_ENTRY(lib_coroutine_ce,"Lib","Coroutine",lib_coroutine_util_methods);
    lib_coroutine_ce_ptr = zend_register_internal_class(&lib_coroutine_ce TSRMLS_CC);
}