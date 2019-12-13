
#include "php_shamem.h"
#include "../include/lib_shamem.h"

/* For compatibility with older PHP versions */
#ifndef ZEND_PARSE_PARAMETERS_NONE
#define ZEND_PARSE_PARAMETERS_NONE() \
	ZEND_PARSE_PARAMETERS_START(0, 0) \
	ZEND_PARSE_PARAMETERS_END()
#endif



/* {{{ void shamem_test1()
 */
PHP_FUNCTION(shamem_test1)
{
	ZEND_PARSE_PARAMETERS_NONE();

	php_printf("The extension %s is loaded and working!\r\n", "shamem");
}
PHP_FUNCTION(shamalloc)
{
	shm_t shm;
	size_t size;

	size = 10 ;//分配8字节内存

	shm.size = size;
	shm.name  = (u_char *)"shared_zone";
	shm_alloc(&shm);

//	shm_free(&shm);
//	int* count = (int *)(shm.addr + 8);
//	zval *return_value;
//	array_init(return_value);
//	zval *arr =  (zval *)shm.addr;
	zend_string *retval;
	retval = (zend_string *)shm.addr;
	php_printf("size %d",sizeof(shm.addr));
	RETURN_STR(retval);
//	array_init(arr);
//	add_assoc_long(arr, "life", 42);
//	return_value = arr;
	php_printf("shammalloc ");

}
/* }}} */

/* {{{ string shamem_test2( [ string $var ] )
 */
PHP_FUNCTION(shamem_test2)
{
	char *var = "World";
	size_t var_len = sizeof("World") - 1;
	zend_string *retval;

	ZEND_PARSE_PARAMETERS_START(0, 1)
		Z_PARAM_OPTIONAL
		Z_PARAM_STRING(var, var_len)
	ZEND_PARSE_PARAMETERS_END();

	retval = strpprintf(0, "Hello %s", var);

	RETURN_STR(&retval);
}
/* }}}*/

/* {{{ PHP_RINIT_FUNCTION
 */
PHP_RINIT_FUNCTION(shamem)
{
#if defined(ZTS) && defined(COMPILE_DL_SHAMEM)
	ZEND_TSRMLS_CACHE_UPDATE();
#endif

	return SUCCESS;
}
/* }}} */

/* {{{ PHP_MINFO_FUNCTION
 */
PHP_MINFO_FUNCTION(shamem)
{
	php_info_print_table_start();
	php_info_print_table_header(2, "shamem support", "enabled");
	php_info_print_table_end();
}
/* }}} */

/* {{{ arginfo
 */
ZEND_BEGIN_ARG_INFO(arginfo_shamem_test1, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_shamem_test2, 0)
	ZEND_ARG_INFO(0, str)
ZEND_END_ARG_INFO()
/* }}} */

/* {{{ shamem_functions[]
 */
static const zend_function_entry shamem_functions[] = {
	PHP_FE(shamalloc,NULL)
	PHP_FE(shamem_test1,		arginfo_shamem_test1)
	PHP_FE(shamem_test2,		arginfo_shamem_test2)
	PHP_FE_END
};
/* }}} */

/* {{{ shamem_module_entry
 */
zend_module_entry shamem_module_entry = {
	STANDARD_MODULE_HEADER,
	"shamem",					/* Extension name */
	shamem_functions,			/* zend_function_entry */
	NULL,							/* PHP_MINIT - Module initialization */
	NULL,							/* PHP_MSHUTDOWN - Module shutdown */
	PHP_RINIT(shamem),			/* PHP_RINIT - Request initialization */
	NULL,							/* PHP_RSHUTDOWN - Request shutdown */
	PHP_MINFO(shamem),			/* PHP_MINFO - Module info */
	PHP_SHAMEM_VERSION,		/* Version */
	STANDARD_MODULE_PROPERTIES
};
/* }}} */

#ifdef COMPILE_DL_SHAMEM
# ifdef ZTS
ZEND_TSRMLS_CACHE_DEFINE()
# endif
ZEND_GET_MODULE(shamem)
#endif

