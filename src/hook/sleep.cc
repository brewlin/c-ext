#include "hook.h"

extern PHP_METHOD(lib_coroutine_util, sleep);

void hook_sleep()
{
    zif_handler new_handler = zim_lib_coroutine_util_sleep;
    zend_function *ori_f = (zend_function *) zend_hash_str_find_ptr(EG(function_table),ZEND_STRL("sleep"));
    ori_f->internal_function.handler = new_handler;
}