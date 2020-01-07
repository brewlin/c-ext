#include "runtime.h"
#include "socket_co.h"



using lib::coroutine::Socket;


ZEND_BEGIN_ARG_INFO_EX(arginfo_lib_runtime_void, 0, 0, 0)
ZEND_END_ARG_INFO()

extern PHP_METHOD(lib_coroutine_util, sleep);


static void hook_func(const char *name, size_t name_len, zif_handler new_handler)
{
    zend_function *ori_f = (zend_function *) zend_hash_str_find_ptr(EG(function_table), name, name_len);
    ori_f->internal_function.handler = new_handler;
}

static int socket_set_option(php_stream *stream, int option, int value, void *ptrparam)
{
    php_lib_netstream_data_t *abstract = (php_lib_netstream_data_t *) stream->abstract;
    php_stream_xport_param *xparam;

    switch(option)
    {
        case PHP_STREAM_OPTION_XPORT_API:
            xparam = (php_stream_xport_param *)ptrparam;

            switch(xparam->op)
            {
                case STREAM_XPORT_OP_BIND:
                    xparam->outputs.returncode = php_lib_tcp_sockop_bind(stream, abstract, xparam);
                    return PHP_STREAM_OPTION_RETURN_OK;
                case STREAM_XPORT_OP_LISTEN:
                    xparam->outputs.returncode = abstract->socket->listen(xparam->inputs.backlog);
                    return PHP_STREAM_OPTION_RETURN_OK;
                default:
                    /* fall through */
                    ;
            }
    }
    return PHP_STREAM_OPTION_RETURN_OK;
}
static int php_lib_tcp_sockop_bind(php_stream *stream, php_lib_netstream_data_t *abstract, php_stream_xport_param *xparam)
{
    char *host = NULL;
    int portno;

    host = parse_ip_address(xparam, &portno);

    if (host == NULL)
    {
        return -1;
    }

    int ret = abstract->socket->bind(LIB_SOCK_TCP, host, portno);

    if (host)
    {
        efree(host);
    }
    return ret;
}
static int socket_close(php_stream *stream, int close_handle)
{
    php_lib_netstream_data_t *abstract = (php_lib_netstream_data_t *) stream->abstract;
    Socket *sock = (Socket*) abstract->socket;

    sock->close();
    delete sock;
    efree(abstract);
    return 0;
}
php_stream *socket_create(const char *proto, size_t protolen,
                          const char *resourcename, size_t resourcenamelen,
                          const char *persistent_id, int options, int flags,
                          struct timeval *timeout,
                          php_stream_context *context STREAMS_DC
){
    php_stream *stream;
    php_lib_netstream_data_t *abstract;
    Socket *sock;

    sock = new Socket(AF_INET, SOCK_STREAM, 0);
    abstract = (php_lib_netstream_data_t*) ecalloc(1, sizeof(*abstract));
    abstract->socket = sock;
    abstract->stream.socket = sock->get_fd();

    if (timeout)
    {
        abstract->stream.timeout = *timeout;
    }
    else
    {
        abstract->stream.timeout.tv_sec = -1;
    }

    persistent_id = nullptr;
    stream = php_stream_alloc_rel(&socket_ops, abstract, persistent_id, "r+");
    if (stream == NULL)
    {
        delete sock;
    }
    return stream;
}


static PHP_METHOD(lib_runtime, enableCoroutine)
{
    hook_func(ZEND_STRL("sleep"), zim_lib_coroutine_util_sleep);
    php_stream_xport_register("tcp",socket_create);
}

static const zend_function_entry lib_runtime_methods[] =
        {
                PHP_ME(lib_runtime, enableCoroutine, arginfo_lib_runtime_void, ZEND_ACC_PUBLIC | ZEND_ACC_STATIC)
                PHP_FE_END
        };
/**
 * Define zend class entry
 */
zend_class_entry lib_runtime_ce;
zend_class_entry *lib_runtime_ce_ptr;

void lib_runtime_init()
{
    INIT_NS_CLASS_ENTRY(lib_runtime_ce, "Lib", "Runtime", lib_runtime_methods);
    lib_runtime_ce_ptr = zend_register_internal_class(&lib_runtime_ce TSRMLS_CC); // Registered in the Zend Engine
}