#include "php_lib.h"
#include "fork.h"
PHP_MINIT_FUNCTION(lib)
{
	lib_coroutine_util_init();
	//注册类
	lib_sharemem_util_init();
   // php_printf("MINIT\n");
	return SUCCESS;
}

PHP_MSHUTDOWN_FUNCTION(lib)
{
    //php_printf("MSHUTDOWN\n");
	return SUCCESS;
}

PHP_RINIT_FUNCTION(lib)
{
    //php_printf("RINIT\n");
	return SUCCESS;
}

PHP_RSHUTDOWN_FUNCTION(lib)
{
    //php_printf("RSHUTDOWN\n");
	return SUCCESS;
}

PHP_MINFO_FUNCTION(lib)
{
	php_info_print_table_start();
	php_info_print_table_header(2, "lib support", "enabled");
	php_info_print_table_row(2, "Author", "Brewlin Team <97404667@qq.com>");
	php_info_print_table_row(2, "Version", "1.0.0");
	php_info_print_table_end();
}

const zend_function_entry lib_functions[] = {
    PHP_FE(lib_fork,NULL)
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