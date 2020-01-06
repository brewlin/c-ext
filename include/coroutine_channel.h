//
// Created by Administrator on 2020/1/6 0006.
//

#ifndef C_EXT_COROUTINE_CHANNEL_H
#define C_EXT_COROUTINE_CHANNEL_H

#include "lib.h"
#include "coroutine.h"
#include <iostream>
#include <queue>


namespace lib {namespace  coroutine {

    class Channel
    {
    public:
        enum opcode
        {
            PUSH =1,
            POP = 2,
        };
        Channel(size_t _capacity = 1);
        ~Channel();
        void *pop(double timeout = -1);
        bool push(void *data,double timeout = -1);

    protected:
        size_t capacity = 1;
        bool closed = false;
        std::queue<Coroutine*> producer_queue;
        std::queue<Coroutine*> consumer_queue;
        std::queue<void*> data_queue;
    };



}}


#endif //C_EXT_COROUTINE_CHANNEL_H
