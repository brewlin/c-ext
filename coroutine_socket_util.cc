#include "socket_co.h"
#include "socket.h"
#include "error.h"
#include "php_lib.h"

using lib::coroutine::Socket;

typedef struct
{
    Socket *socket;
    zend_object std;

}co_socket;

ZEND_BEGIN_ARG_INFO_EX(arginfo_co_socket_void, 0, 0, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_co_socket_construct, 0, 0, 2)
ZEND_ARG_INFO(0, domain)
ZEND_ARG_INFO(0, type)
ZEND_ARG_INFO(0, protocol)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_co_socket_bind, 0, 0, 2)
ZEND_ARG_INFO(0, host)
ZEND_ARG_INFO(0, port)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_co_socket_listen, 0, 0, 0)
ZEND_ARG_INFO(0, backlog)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_co_socket_recv, 0, 0, 0)
ZEND_ARG_INFO(0, length)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_co_socket_send, 0, 0, 1)
ZEND_ARG_INFO(0, data)
ZEND_END_ARG_INFO()

/**
 * Define zend class entry
 */
zend_class_entry co_socket_ce;
zend_class_entry *co_socket_ce_ptr;

static zend_object_handlers co_socket_handlers;


static co_socket* co_socket_fetch_object(zend_object *obj)
{
    return (co_socket *) ((char *) obj - co_socket_handlers.offset);
}

static zend_object* co_socket_create_object(zend_class_entry *ce)
{
    co_socket *sock = (co_socket *) ecalloc(1, sizeof(co_socket) + zend_object_properties_size(ce));
    zend_object_std_init(&sock->std, ce);
    object_properties_init(&sock->std, ce);
    sock->std.handlers = &co_socket_handlers;
    return &sock->std;
}

static void co_socket_free_object(zend_object *object)
{
    co_socket *sock = (co_socket *) co_socket_fetch_object(object);
    if (sock->socket && sock->socket != LIB_BAD_SOCKET)
    {
        sock->socket->close();
        delete sock->socket;
    }
    zend_object_std_dtor(&sock->std);
}

void php_lib_init_socket_object(zval *zsocket, Socket *socket)
{
    zend_object *object = co_socket_create_object(co_socket_ce_ptr);

    co_socket *socket_t = (co_socket *) co_socket_fetch_object(object);
    socket_t->socket = socket;
    ZVAL_OBJ(zsocket, object);
}


static PHP_METHOD(co_socket, __construct)
{
    zend_long domain;
    zend_long type;
    zend_long protocol;
    co_socket *socket_t;

    ZEND_PARSE_PARAMETERS_START(2, 3)
    Z_PARAM_LONG(domain)
    Z_PARAM_LONG(type)
    Z_PARAM_OPTIONAL
            Z_PARAM_LONG(protocol)
    ZEND_PARSE_PARAMETERS_END_EX(RETURN_FALSE);

    socket_t = (co_socket *)co_socket_fetch_object(Z_OBJ_P(getThis()));
    socket_t->socket = new Socket(domain, type, protocol);

    zend_update_property_long(co_socket_ce_ptr, getThis(), ZEND_STRL("fd"), socket_t->socket->get_fd());
}

static PHP_METHOD(co_socket, bind)
{
    zend_long port;
    Socket *socket;
    co_socket *socket_t;
    zval *zhost;

    ZEND_PARSE_PARAMETERS_START(2, 2)
    Z_PARAM_ZVAL(zhost)
    Z_PARAM_LONG(port)
    ZEND_PARSE_PARAMETERS_END_EX(RETURN_FALSE);

    socket_t = (co_socket *)co_socket_fetch_object(Z_OBJ_P(getThis()));
    socket = socket_t->socket;

    if (socket->bind(LIB_SOCK_TCP, Z_STRVAL_P(zhost), port) < 0)
    {
        RETURN_FALSE;
    }
    RETURN_TRUE;
}

static PHP_METHOD(co_socket, listen)
{
    zend_long backlog = 512;
    Socket *socket;
    co_socket *socket_t;

    ZEND_PARSE_PARAMETERS_START(0, 1)
    Z_PARAM_OPTIONAL
    Z_PARAM_LONG(backlog)
    ZEND_PARSE_PARAMETERS_END_EX(RETURN_FALSE);

    socket_t = (co_socket *)co_socket_fetch_object(Z_OBJ_P(getThis()));
    socket = socket_t->socket;

    if (socket->listen(backlog) < 0)
    {
        RETURN_FALSE;
    }
    RETURN_TRUE;
}

static PHP_METHOD(co_socket, accept)
{
    co_socket *server_socket_t;
    co_socket *conn_socket_t;
    Socket *server_socket;
    Socket *conn_socket;

    server_socket_t = (co_socket *)co_socket_fetch_object(Z_OBJ_P(getThis()));
    server_socket = server_socket_t->socket;
    conn_socket = server_socket->accept();
    if (!conn_socket)
    {
        RETURN_NULL();
    }
    zend_object *conn = co_socket_create_object(co_socket_ce_ptr);
    conn_socket_t = (co_socket *)co_socket_fetch_object(conn);
    conn_socket_t->socket = conn_socket;
    ZVAL_OBJ(return_value, &(conn_socket_t->std));
    zend_update_property_long(co_socket_ce_ptr, return_value, ZEND_STRL("fd"), conn_socket_t->socket->get_fd());
}
static PHP_METHOD(co_socket, recv)
{
    ssize_t ret;
    zend_long length = 65536;
    co_socket *conn_socket_t;
    Socket *conn_socket;

    ZEND_PARSE_PARAMETERS_START(0, 1)
    Z_PARAM_OPTIONAL
    Z_PARAM_LONG(length)
    ZEND_PARSE_PARAMETERS_END_EX(RETURN_FALSE);

    Socket::init_read_buffer();

    conn_socket_t = (co_socket *)co_socket_fetch_object(Z_OBJ_P(getThis()));
    conn_socket = conn_socket_t->socket;
    ret = conn_socket->recv(Socket::read_buffer, Socket::read_buffer_len);
    if (ret == 0)
    {
        zend_update_property_long(co_socket_ce_ptr, getThis(), ZEND_STRL("errCode"), LIB_ERROR_SESSION_CLOSED_BY_CLIENT);
        zend_update_property_string(co_socket_ce_ptr, getThis(), ZEND_STRL("errMsg"), lib_strerror(LIB_ERROR_SESSION_CLOSED_BY_CLIENT));
        RETURN_FALSE;
    }
    if (ret < 0)
    {
        php_error_docref(NULL, E_WARNING, "recv error");
        RETURN_FALSE;
    }
    Socket::read_buffer[ret] = '\0';
    RETURN_STRING(Socket::read_buffer);
}
static PHP_METHOD(co_socket, send)
{
    ssize_t ret;
    char *data;
    size_t length;
    co_socket *conn_socket_t;
    Socket *conn_socket;

    ZEND_PARSE_PARAMETERS_START(1, 1)
    Z_PARAM_STRING(data, length)
    ZEND_PARSE_PARAMETERS_END_EX(RETURN_FALSE);

    conn_socket_t = (co_socket *)co_socket_fetch_object(Z_OBJ_P(getThis()));
    conn_socket = conn_socket_t->socket;
    ret = conn_socket->send(data, length);
    if (ret < 0)
    {
    php_error_docref(NULL, E_WARNING, "send error");
    RETURN_FALSE;
    }
    RETURN_LONG(ret);
}
static PHP_METHOD(co_socket, close)
{
    int ret;
    co_socket *conn_socket_t;

    conn_socket_t = (co_socket *)co_socket_fetch_object(Z_OBJ_P(getThis()));
    ret = conn_socket_t->socket->close();
    if (ret < 0)
    {
    php_error_docref(NULL, E_WARNING, "close error");
    RETURN_FALSE;
    }
    delete conn_socket_t->socket;
    conn_socket_t->socket = LIB_BAD_SOCKET;

    RETURN_TRUE;
    }
static const zend_function_entry co_socket_methods[] =
        {
                PHP_ME(co_socket, __construct, arginfo_co_socket_construct, ZEND_ACC_PUBLIC | ZEND_ACC_CTOR)
                PHP_ME(co_socket, bind, arginfo_co_socket_bind, ZEND_ACC_PUBLIC)
                PHP_ME(co_socket, listen, arginfo_co_socket_listen, ZEND_ACC_PUBLIC)
                PHP_ME(co_socket, accept, arginfo_co_socket_void, ZEND_ACC_PUBLIC)
                PHP_ME(co_socket, recv, arginfo_co_socket_recv, ZEND_ACC_PUBLIC)
                PHP_ME(co_socket, send, arginfo_co_socket_send, ZEND_ACC_PUBLIC)
                PHP_ME(co_socket, close, arginfo_co_socket_void, ZEND_ACC_PUBLIC)
                PHP_FE_END
        };

void lib_co_socket_init(int module_number)
{
    INIT_NS_CLASS_ENTRY(co_socket_ce, "Lib", "Coroutine\\Socket", co_socket_methods);
    memcpy(&co_socket_handlers, zend_get_std_object_handlers(), sizeof(zend_object_handlers));
    co_socket_ce_ptr = zend_register_internal_class(&co_socket_ce TSRMLS_CC); // Registered in the Zend Engine
    REGISTER_CUSTOM_OBJECT(co_socket, co_socket_create_object, co_socket_free_object,std);

    zend_declare_property_long(co_socket_ce_ptr, ZEND_STRL("fd"), -1, ZEND_ACC_PUBLIC);
    zend_declare_property_long(co_socket_ce_ptr, ZEND_STRL("errCode"), 0, ZEND_ACC_PUBLIC);
    zend_declare_property_string(co_socket_ce_ptr, ZEND_STRL("errMsg"), "", ZEND_ACC_PUBLIC);

    if (!zend_hash_str_find_ptr(&module_registry, ZEND_STRL("sockets")))
    {
        REGISTER_LONG_CONSTANT("AF_INET", AF_INET, CONST_CS  | CONST_PERSISTENT);
        REGISTER_LONG_CONSTANT("SOCK_STREAM", SOCK_STREAM, CONST_CS | CONST_PERSISTENT);
        REGISTER_LONG_CONSTANT("IPPROTO_IP", IPPROTO_IP, CONST_CS | CONST_PERSISTENT);
    }
}