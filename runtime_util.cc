#include "runtime.h"

ZEND_BEGIN_ARG_INFO_EX(arginfo_lib_runtime_void, 0, 0, 0)
ZEND_END_ARG_INFO()

extern PHP_METHOD(lib_coroutine_util, sleep);

static void hook_func(const char *name, size_t name_len, zif_handler new_handler)
{
    zend_function *ori_f = (zend_function *) zend_hash_str_find_ptr(EG(function_table), name, name_len);
    ori_f->internal_function.handler = new_handler;
}

static PHP_METHOD(lib_runtime, enableCoroutine)
{
    hook_func(ZEND_STRL("sleep"), zim_lib_coroutine_util_sleep);
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