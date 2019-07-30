#include "php_lib.h"

// zend_class_entry lib_coroutine_ce;
// zend_class_entry *lib_coroutine_ce_ptr;

PHP_MINIT_FUNCTION(lib)
{
	// INIT_NS_CLASS_ENTRY(lib_coroutine_ce,"Lib","Coroutine",lib_coroutine_util_methods)
	lib_coroutine_util_init();
    php_printf("MINIT\n");
	return SUCCESS;
}

PHP_MSHUTDOWN_FUNCTION(lib)
{
    php_printf("MSHUTDOWN\n");
	return SUCCESS;
}

PHP_RINIT_FUNCTION(lib)
{
    php_printf("RINIT\n");
	return SUCCESS;
}

PHP_RSHUTDOWN_FUNCTION(lib)
{
    php_printf("RSHUTDOWN\n");
	return SUCCESS;
}

PHP_MINFO_FUNCTION(lib)
{
	php_info_print_table_start();
	php_info_print_table_header(2, "lib support", "enabled");
	php_info_print_table_end();
}

const zend_function_entry lib_functions[] = {
	PHP_FE_END
};

zend_module_entry lib_module_entry = {
	STANDARD_MODULE_HEADER,
	"lib",
	lib_functions,
	PHP_MINIT(lib),
	PHP_MSHUTDOWN(lib),
	PHP_RINIT(lib),
	PHP_RSHUTDOWN(lib),
	PHP_MINFO(lib),
	PHP_LIB_VERSION,
	STANDARD_MODULE_PROPERTIES
};

#ifdef COMPILE_DL_LIB
ZEND_GET_MODULE(lib)
#endif