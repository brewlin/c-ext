#ifndef PHP_LIB_H
#define PHP_LIB_H

#include "php.h"
#include "php_ini.h"
#include "ext/standard/info.h"

#include "lib.h"

#define PHP_LIB_VERSION "0.1.0"

extern zend_module_entry lib_module_entry;
#define phpext_lib_ptr &lib_module_entry

#ifdef PHP_WIN32
#	define PHP_LIB_API __declspec(dllexport)
#elif defined(__GNUC__) && __GNUC__ >= 4
#	define PHP_LIB_API __attribute__ ((visibility("default")))
#else
#	define PHP_LIB_API
#endif

#ifdef ZTS
#include "TSRM.h"
#endif

/**
 * Declare any global variables you may need between the BEGIN and END macros here
 */
ZEND_BEGIN_MODULE_GLOBALS(lib)

ZEND_END_MODULE_GLOBALS(lib)

void lib_coroutine_util_init();
void lib_sharemem_util_init();
void lib_process_init();
void lib_coroutine_server_coro_init();


PHP_FUNCTION(lib_coroutine_create);



inline zval *lib_zend_read_property(zend_class_entry *class_ptr, zval *obj, const char *s, int len, int silent)
{
    zval rv;
    return zend_read_property(class_ptr, obj, s, len, silent, &rv);
}

#endif	/* PHP_LIB_H */