#include "lib_coroutine.h"

ZEND_BEGIN_ARG_INFO_EX(arginfo_lib_coroutine_create,0,0,1)
ZEND_ARG_CALLABLE_INFO(0,func,0)
ZEND_END_ARG_INFO()

//声明方法
static PHP_METHOD(lib_coroutine_util,create);


PHP_METHOD(lib_coroutine_util,create)
{
    php_printf("lib_coroutine_util,create\n");
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