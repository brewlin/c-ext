#ifndef LIB_COROUTINE_H
#define LIB_COROUTINE_H

#include "php_lib.h"

namespace Lib
{
    class PHPCoroutine
    {
        static long create(zend_fcall_info_cache *fci_cache,uint32_t argc,zval *argv);
    };
}

#endif // !1LIB_COROUTINE_#define LIB_COROUTINE_HH
