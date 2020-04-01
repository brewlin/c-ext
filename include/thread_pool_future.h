//
// Created by Administrator on 2020/3/31 0031.
//

#ifndef C_EXT_THREAD_POOL_FUTURE_H
#define C_EXT_THREAD_POOL_FUTURE_H

#include <future>
#include "php_lib.h"
#include "lib_class.h"
#include <memory>
/**
 * Define zend class entry
 */
extern   zend_class_entry thread_pool_future_ce;
extern   zend_class_entry *thread_pool_future_ce_ptr;
extern   zend_object_handlers thread_pool_future_handlers;

struct thread_pool_future
{
    future<void *> fu;
    zend_object std;

    FETCH_METHOD(thread_pool_future)
    CREATE_METHOD(thread_pool_future)
    static void free_object(zend_object *object)
    {
        auto obj =fetch_object(object);
        zend_object_std_dtor(&obj->std);
    }
    static void init_object(zval *z, future<void *> obj)
    {
        zend_object *object = create_object(thread_pool_future_ce_ptr);
        thread_pool_future *cobj = fetch_object(object);
        cobj->fu = std::move(obj);
        ZVAL_OBJ(z, object);
    }
};



#endif //C_EXT_THREAD_POOL_FUTURE_H
