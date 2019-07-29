#include "php_coroutine.h"

ZEND_BEGIN_ARG_INFO_EX(arginfo_php_coroutine_create,0,0,1)
ZEND_ARG_CALLABLE_INFO(0,func,0)
ZEND_END_ARG_INFO()

//声明这个方法
static PHP_METHOD(php_coroutine_util,create);

PHP_METHOD(php_coroutine_util,create)
{
    php_printf("success!\n");
}

const zend_function_entry php_coroutine_util_methods[] = 
{
    PHP_ME(php_coroutine_util,create,arginfo_php_coroutine_create,ZEND_ACC_PUBLIC | ZEND_ACC_STATIC)
    PHP_FE_END
};
zend_class_entry php_coroutine_ce;
zend_class_entry *php_coroutine_ce_ptr;
void php_coroutine_util_init()
{
    INIT_NS_CLASS_ENTRY(php_coroutine_ce,"Lib","Coroutine",php_coroutine_util_methods);
    php_coroutine_ce_ptr = zend_register_internal_class(&php_coroutine_ce TSRMLS_CC);
}
