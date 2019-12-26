//
// Created by Administrator on 2019/12/26 0026.
//

#ifndef C_EXT_SOCKET_CO_H
#define C_EXT_SOCKET_CO_H

#include "lib.h"

namespace lib {
    namespace coroutine{
        class Socket
        {

        private:
            int sockfd;
        public:
            Socket(int domain,int type,int protocol);
            Socket(int fd);
            ~Socket();
            int bind(int type,char *host,int port);
            int listen();
            int accept();
            ssize_t recv(void *buf, size_t len);
            ssize_t send(const void *buf, size_t len);
            int close();

            bool wait_event(int event);


        };
    }
}

#endif //C_EXT_SOCKET_CO_H
