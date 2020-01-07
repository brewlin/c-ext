#include "coroutine_server.h"
#include "socket_co.h"
#include "log.h"

using lib::phpcoroutine::Server;
using lib::PHPCoroutine;
using lib::coroutine::Socket;

Server::Server(char *host, int port)
{
    socket = new Socket(AF_INET, SOCK_STREAM, 0);
    if (socket->bind(LIB_SOCK_TCP, host, port) < 0)
    {
        libWarn("Error has occurred: (errno %d) %s", errno, strerror(errno));
        return;
    }
    if (socket->listen(512) < 0)
    {
        return;
    }
}
Server::~Server()
{
}

bool Server::start()
{
    zval zsocket;
    running = true;

    while (running)
    {
        Socket* conn = socket->accept();
        if (!conn)
        {
            return false;
        }

        php_lib_init_socket_object(&zsocket, conn);
        PHPCoroutine::create(&(handler->fcc), 1, &zsocket);
        zval_dtor(&zsocket);
    }
    return true;
}

bool Server::shutdown()
{
    running = false;
    return true;
}
void Server::set_handler(php_lib_fci_fcc *_handler)
{
    handler = _handler;
}
php_lib_fci_fcc* Server::get_handler()
{
    return handler;
}