#ifndef _C_EXT_ZEND_CALLBACK_H_
#define _C_EXT_ZEND_CALLBACK_H_

#include "php_lib.h"
#include <iostream>
using namespace std;

//回调函数用户空间内存释放
inline void zend_fci_cache_discard(zend_fcall_info_cache *fci_cache)
{
    if (fci_cache->object) {
        OBJ_RELEASE(fci_cache->object);
    }
    if (fci_cache->function_handler->op_array.fn_flags & ZEND_ACC_CLOSURE) {
        OBJ_RELEASE(ZEND_CLOSURE_OBJECT(fci_cache->function_handler));

    }
}
inline void zend_fci_params_discard(zend_fcall_info *fci)
{
    if (fci->param_count > 0)
    {
        uint32_t i;
        for (i = 0; i < fci->param_count; i++)
        {
            zval_ptr_dtor(&fci->params[i]);
        }
        efree(fci->params);
    }
}

inline void zend_fci_cache_persist(zend_fcall_info_cache *fci_cache)
{
    if (fci_cache->object)
    {
        GC_ADDREF(fci_cache->object);
    }
    if (fci_cache->function_handler->op_array.fn_flags & ZEND_ACC_CLOSURE)
    {
        GC_ADDREF(ZEND_CLOSURE_OBJECT(fci_cache->function_handler));
    }
}


struct CallBackParam
{
    zend_fcall_info fci;
    zend_fcall_info_cache fcc;
    // time event
    long seconds;

    void AddRefCount()
    {
        zend_fci_cache_persist(&fcc);
    }
    void call(zval *result)
    {
        fci.retval = result;
        if (zend_call_function(&fci, &fcc) != SUCCESS)
        {
            php_error_docref(NULL, E_WARNING, "defer execute error");
            return;
        }
    }
    ~CallBackParam()
    {
        zend_fci_cache_discard(&fcc);
        zend_fci_params_discard(&fci);
    }
};


#endif 