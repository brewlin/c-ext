#include "server_coro.h"

zend_class_entry lib_coroutine_server_coro_ce;
zend_class_entry *lib_coroutine_server_coro_ce_ptr;

/*************定义参数*********************/
ZEND_BEGIN_ARG_INFO_EX(arginfo_server_coro_construct, 0, 0, 2)
ZEND_ARG_INFO(0, host)
ZEND_ARG_INFO(0, port)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_server_coro_recv, 0, 0, 2)
ZEND_ARG_INFO(0, fd)
ZEND_ARG_INFO(0, length)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_server_coro_accept, 0, 0, 0)
ZEND_END_ARG_INFO()

/*************定义方法********************/

PHP_METHOD(server_obj,__construct)
{
    int sock;
    zval *zhost;
    zend_long zport;

    ZEND_PARSE_PARAMETERS_START(2,2)
        Z_PARAM_ZVAL(zhost)
        Z_PARAM_LONG(zport)
    ZEND_PARSE_PARAMETERS_END_EX(RETURN_FALSE);

    sock = libsocket_create(LIB_SOCK_TCP);
    //将zhost 转换为字符串
    libsocket_bind(sock,LIB_SOCK_TCP,Z_STRVAL_P(zhost),zport);
    libsocket_listen(sock);
    //更新类的属性
    zend_update_property_long(lib_coroutine_server_coro_ce_ptr, getThis(), ZEND_STRL("sock"), sock);
    zend_update_property_string(lib_coroutine_server_coro_ce_ptr, getThis(), ZEND_STRL("host"), Z_STRVAL_P(zhost));
    zend_update_property_long(lib_coroutine_server_coro_ce_ptr, getThis(), ZEND_STRL("port"), zport);
}

PHP_METHOD(server_obj, accept)
{
    zval *zsock;
    int connfd;

    //读取对象上的属性
    zsock = lib_zend_read_property(lib_coroutine_server_coro_ce_ptr, getThis(), ZEND_STRL("sock"), 0);
    connfd = libsocket_accept(Z_LVAL_P(zsock));
    RETURN_LONG(connfd);
}

PHP_METHOD(server_obj, recv)
{
    int ret;
    zend_long fd;
    zend_long length = 65536;
    //可选参数
    ZEND_PARSE_PARAMETERS_START(1, 2)
        Z_PARAM_LONG(fd)
        Z_PARAM_OPTIONAL
        Z_PARAM_LONG(length)
    ZEND_PARSE_PARAMETERS_END_EX(RETURN_FALSE);
    //申请内存 buf 底层自动 lenght + 1
    zend_string *buf = zend_string_alloc(length, 0);

    ret = libsocket_recv(fd, ZSTR_VAL(buf), length, 0);
    if (ret < 0)
    {
        php_error_docref(NULL, E_WARNING, "recv error");
        RETURN_FALSE;
    }
    ZSTR_VAL(buf)[ret] = '\0';
    RETURN_STR(buf);
}

/*************注册函数***********************/
static const zend_function_entry lib_server_coro_methods[] =
        {
                PHP_ME(server_obj, __construct, arginfo_server_coro_construct, ZEND_ACC_PUBLIC | ZEND_ACC_CTOR) // ZEND_ACC_CTOR is used to declare that this method is a constructor of this class.
                PHP_ME(server_obj, recv, arginfo_server_coro_recv, ZEND_ACC_PUBLIC | ZEND_ACC_CTOR) // ZEND_ACC_CTOR is used to declare that this method is a constructor of this class.
                PHP_ME(server_obj, accept, arginfo_server_coro_accept, ZEND_ACC_PUBLIC | ZEND_ACC_CTOR) // ZEND_ACC_CTOR is used to declare that this method is a constructor of this class.
                PHP_FE_END
        };

void lib_coroutine_server_coro_init()
{
    INIT_NS_CLASS_ENTRY(lib_coroutine_server_coro_ce, "Lib", "Coroutine\\Server", lib_server_coro_methods);
    lib_coroutine_server_coro_ce_ptr = zend_register_internal_class(&lib_coroutine_server_coro_ce TSRMLS_CC);
    // Registered in the Zend Engine
    zend_declare_property_long(lib_coroutine_server_coro_ce_ptr, ZEND_STRL("sock"), -1, ZEND_ACC_PUBLIC);
    zend_declare_property_string(lib_coroutine_server_coro_ce_ptr, ZEND_STRL("host"), "", ZEND_ACC_PUBLIC);
    zend_declare_property_long(lib_coroutine_server_coro_ce_ptr, ZEND_STRL("port"), -1, ZEND_ACC_PUBLIC);
}