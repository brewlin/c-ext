#include "runtime.h"

ZEND_BEGIN_ARG_INFO_EX(arginfo_lib_runtime_void, 0, 0, 0)
ZEND_END_ARG_INFO()


static PHP_METHOD(lib_runtime, enableCoroutine)
{
    hook_sleep();
    hook_socket_stream();
}

static const zend_function_entry lib_runtime_methods[] =
        {
                PHP_ME(lib_runtime, enableCoroutine, arginfo_lib_runtime_void, ZEND_ACC_PUBLIC | ZEND_ACC_STATIC)
                PHP_FE_END
        };
/**
 * Define zend class entry
 */
zend_class_entry lib_runtime_ce;
zend_class_entry *lib_runtime_ce_ptr;

void lib_runtime_init()
{
    INIT_NS_CLASS_ENTRY(lib_runtime_ce, "Lib", "Runtime", lib_runtime_methods);
    lib_runtime_ce_ptr = zend_register_internal_class(&lib_runtime_ce TSRMLS_CC); // Registered in the Zend Engine
}