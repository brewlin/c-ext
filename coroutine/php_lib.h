#ifndef PHP_LIB_H
#define PHP_LIB_H

#include "php.h"
#include "php_ini.h"
#include "ext/standard/info.h"

#include "study.h"

#define PHP_STUDY_VERSION "0.1.0"

extern zend_module_entry study_module_entry;
#define phpext_study_ptr &study_module_entry

#ifdef PHP_WIN32
#	define PHP_STUDY_API __declspec(dllexport)
#elif defined(__GNUC__) && __GNUC__ >= 4
#	define PHP_STUDY_API __attribute__ ((visibility("default")))
#else
#	define PHP_STUDY_API
#endif

#ifdef ZTS
#include "TSRM.h"
#endif

/**
 * Declare any global variables you may need between the BEGIN and END macros here
 */
ZEND_BEGIN_MODULE_GLOBALS(study)

ZEND_END_MODULE_GLOBALS(study)

#endif	/* PHP_STUDY_H */