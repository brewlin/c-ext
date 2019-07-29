#ifndef LIB_COROUTINE_H
#define LIB_COROUTINE_H

#include "php_study.h"
namespace Coroutine
{
    class PHPCoroutine
    {
        static long create(zend_fcall_info_cache *fci_cache,uint32_t argc,zval *argv);
    };
}


#endif	/* PHP_COROUTINE_H */