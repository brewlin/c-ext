//
// Created by Administrator on 2020/1/7 0007.
//

#ifndef C_EXT_RUNTIME_H
#define C_EXT_RUNTIME_H

#include "php_lib.h"
#include "socket_co.h"
#include "socket.h"

using namespace lib::coroutine;




struct php_lib_netstream_data_t
{
    php_netstream_data_t stream;
    Socket *socket;
};
static int socket_close(php_stream *stream, int close_handle);
static int socket_set_option(php_stream *stream, int option, int value, void *ptrparam);
static int php_lib_tcp_sockop_bind(php_stream *stream, php_lib_netstream_data_t *abstract, php_stream_xport_param *xparam);



static php_stream_ops socket_ops
        {
                NULL,
                NULL,
                socket_close,
                NULL,
                "tcp_socket/coroutine",
                NULL, /* seek */
                NULL,
                NULL,
                socket_set_option,
        };
enum
{
    STREAM_XPORT_OP_BIND,
    STREAM_XPORT_OP_CONNECT,
    STREAM_XPORT_OP_LISTEN,
    STREAM_XPORT_OP_ACCEPT,
    STREAM_XPORT_OP_CONNECT_ASYNC,
    STREAM_XPORT_OP_GET_NAME,
    STREAM_XPORT_OP_GET_PEER_NAME,
    STREAM_XPORT_OP_RECV,
    STREAM_XPORT_OP_SEND,
    STREAM_XPORT_OP_SHUTDOWN,
};

/**
 * copy from php src file: xp_socket.c
 */
static inline char *parse_ip_address_ex(const char *str, size_t str_len, int *portno, int get_err, zend_string **err)
{
    char *colon;
    char *host = NULL;

#ifdef HAVE_IPV6
    char *p;

    if (*(str) == '[' && str_len > 1)
    {
        /* IPV6 notation to specify raw address with port (i.e. [fe80::1]:80) */
        p = (char *)memchr(str + 1, ']', str_len - 2);
        if (!p || *(p + 1) != ':')
        {
            if (get_err)
            {
                *err = strpprintf(0, "Failed to parse IPv6 address \"%s\"", str);
            }
            return NULL;
        }
        *portno = atoi(p + 2);
        return estrndup(str + 1, p - str - 1);
    }
#endif
    if (str_len)
    {
        colon = (char *)memchr(str, ':', str_len - 1);
    }
    else
    {
        colon = NULL;
    }
    if (colon)
    {
        *portno = atoi(colon + 1);
        host = estrndup(str, colon - str);
    }
    else
    {
        if (get_err)
        {
            *err = strpprintf(0, "Failed to parse address \"%s\"", str);
        }
        return NULL;
    }

    return host;
}

/**
 * copy from php src file: xp_socket.c
 */
static inline char *parse_ip_address(php_stream_xport_param *xparam, int *portno)
{
    return parse_ip_address_ex(xparam->inputs.name, xparam->inputs.namelen, portno, xparam->want_errortext, &xparam->outputs.error_text);
}
#endif //C_EXT_RUNTIME_H
