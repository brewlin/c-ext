//
// Created by Administrator on 2019/12/26 0026.
//

#ifndef C_EXT_SERVER_CORO_H
#define C_EXT_SERVER_CORO_H

#include "php_lib.h"
#include "lib_coroutine.h"
#include "socket.h"
#include "socket_co.h"

#include "error.h"


using namespace lib::coroutine;

namespace lib { namespace phpcoroutine {
        class Server
        {
        private:
            lib::coroutine::Socket *socket = nullptr;
            php_lib_fci_fcc *handler = nullptr;
            bool running = false;

        public:
            Server(char *host, int port);
            ~Server();
            bool start();
            bool shutdown();
            void set_handler(php_lib_fci_fcc *_handler);
            php_lib_fci_fcc* get_handler();
        };
    }
}

void php_lib_init_socket_object(zval *zsocket, Socket *socket);

#endif //C_EXT_SERVER_CORO_H
