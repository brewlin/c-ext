#include "lib_coroutine.h"

zend_class_entry lib_coroutine_ce;
zend_class_entry *lib_coroutine_ce_ptr;

PHP_MINIT_FUNCTION(study)
{
    php_printf("MINIT\n");
	INIT_NS_CLASS_ENTRY(lib_coroutine_ce,"Lib","Coroutine",lib_coroutine_util_methods);
	
	return SUCCESS;
}

PHP_MSHUTDOWN_FUNCTION(study)
{
    php_printf("MSHUTDOWN\n");
	return SUCCESS;
}

PHP_RINIT_FUNCTION(study)
{
    php_printf("RINIT\n");
	return SUCCESS;
}

PHP_RSHUTDOWN_FUNCTION(study)
{
    php_printf("RSHUTDOWN\n");
	return SUCCESS;
}

PHP_MINFO_FUNCTION(study)
{
	php_info_print_table_start();
	php_info_print_table_header(2, "study support", "enabled");
	php_info_print_table_end();
}

const zend_function_entry study_functions[] = {
	PHP_FE_END
};

zend_module_entry study_module_entry = {
	STANDARD_MODULE_HEADER,
	"study",
	study_functions,
	PHP_MINIT(study),
	PHP_MSHUTDOWN(study),
	PHP_RINIT(study),
	PHP_RSHUTDOWN(study),
	PHP_MINFO(study),
	PHP_LIB_VERSION,
	STANDARD_MODULE_PROPERTIES
};

#ifdef COMPILE_DL_STUDY
ZEND_GET_MODULE(study)
#endif