//
// Created by Administrator on 2020/3/31 0031.
//

#ifndef C_EXT_LIB_CLASS_H
#define C_EXT_LIB_CLASS_H

#include "php_lib.h"

// 公用获取对应的php对象
#define FETCH_METHOD(class_name) \
        static class_name* fetch_object(zend_object *obj){  \
            return (class_name *) ((char *) obj - class_name##_handlers.offset); \
        }

#define CREATE_METHOD(class_name)            \
        static zend_object* create_object(zend_class_entry *ce) \
        { \
            class_name *obj = (class_name *) ecalloc(1, sizeof(class_name) + zend_object_properties_size(ce)); \
            zend_object_std_init(&obj->std, ce); \
            object_properties_init(&obj->std, ce); \
            obj->std.handlers = &class_name##_handlers; \
            return &obj->std; \
        }

#define FREE_METHOD(class_name,fobj) \
        static void free_object(zend_object *object) \
        { \
            class_name *obj = (class_name *) fetch_object(object); \
            if (obj->fobj) \
            { \
                delete obj->fobj; \
            } \
            zend_object_std_dtor(&obj->std); \
        }

#define FREE_METHOD_START(class_name,fobj) \
        static void free_object(zend_object *object) \
        { \
            class_name *obj = (class_name *) fetch_object(object); \
            if (obj->fobj) \
            {

#define FREE_MEHTOD_END \
            } \
            zend_object_std_dtor(&obj->std); \
        }

#define INIT_METHOD(class_name,CClass,fobj) \
        static void init_object(zval *z, CClass *obj) \
        { \
            zend_object *object = create_object(class_name##_ce_ptr); \
            class_name *cobj = (class_name *) fetch_object(object); \
            cobj->fobj = obj; \
            ZVAL_OBJ(z, object); \
        }

#define INIT_CLASS_HANDLER(class_name) \
        zend_class_entry class_name##_ce; \
        zend_class_entry *class_name##_ce_ptr; \
        static zend_object_handlers class_name##_handlers;


#define GET_THIS_OBJECT Z_OBJ_P(getThis())

#define THIS_CE(class_name) class_name##_ce
#define THIS_PTR(class_name) class_name##_ce_ptr
#define THIS_HANDLER(class_name) class_name##_handlers


#endif //C_EXT_LIB_CLASS_H
