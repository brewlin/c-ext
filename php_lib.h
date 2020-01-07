#ifndef PHP_LIB_H
#define PHP_LIB_H

#include "php.h"
#include "php_ini.h"
#include "ext/standard/info.h"
#include "php_network.h"
#include "php_streams.h"

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
//void lib_coroutine_server_coro_init();
void lib_co_server_init(int module_number);
void lib_timer_util_init();
void lib_channel_init();
void lib_co_socket_init(int module_number);
void lib_runtime_init();

PHP_FUNCTION(lib_coroutine_create);

PHP_FUNCTION(lib_event_init);
PHP_FUNCTION(lib_event_wait);

inline zval *lib_zend_read_property(zend_class_entry *class_ptr, zval *obj, const char *s, int len, int silent)
{
    zval rv;
    return zend_read_property(class_ptr, obj, s, len, silent, &rv);
}

inline zval* lib_malloc_zval()
{
    return (zval *) emalloc(sizeof(zval));
}

inline zval* lib_zval_dup(zval *val)
{
    zval *dup = lib_malloc_zval();
    memcpy(dup, val, sizeof(zval));
    return dup;
}

#define ST_SET_CLASS_CREATE(module, _create_object) \
    module##_ce_ptr->create_object = _create_object

#define ST_SET_CLASS_FREE(module, _free_obj) \
    module##_handlers.free_obj = _free_obj

#define ST_SET_CLASS_CREATE_AND_FREE(module, _create_object, _free_obj) \
    ST_SET_CLASS_CREATE(module, _create_object); \
    ST_SET_CLASS_FREE(module, _free_obj)

/**
 * module##_handlers.offset 保存PHP对象在自定义对象中的偏移量
 */
#define SET_CLASS_CUSTOM_OBJECT(module, _create_object, _free_obj, _struct, _std) \
    ST_SET_CLASS_CREATE_AND_FREE(module, _create_object, _free_obj); \
    module##_handlers.offset = XtOffsetOf(_struct, _std)

#endif	/* PHP_LIB_H */