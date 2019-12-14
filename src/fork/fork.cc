#include "fork.h"

PHP_FUNCTION(lib_fork)
{
    php_printf( "fork function \n");
    int re = fork();
    ZVAL_LONG(return_value,(long)re);
}