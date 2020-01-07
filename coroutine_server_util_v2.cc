#include "coroutine_server.h"
#include "error.h"
#include "socket_co.h"

using namespace lib::coroutine;
using namespace lib::phpcoroutine;

typedef struct
{
    Server *serv;
    zend_object std;
}co_serv;


zend_class_entry lib_co_server_ce;
zend_class_entry *lib_co_server_ce_ptr;
static zend_object_handlers lib_co_server_handlers;



static co_serv* lib_co_server_fetch_object(zend_object *obj)
{
    return (co_serv *)((char *)obj - lib_co_server_handlers.offset);
}

static zend_object* lib_co_server_create_object(zend_class_entry *ce)
{
    co_serv *serv_t = (co_serv *)ecalloc(1, sizeof(co_serv) + zend_object_properties_size(ce));
    zend_object_std_init(&serv_t->std, ce);
    object_properties_init(&serv_t->std, ce);
    serv_t->std.handlers = &lib_co_server_handlers;
    return &serv_t->std;
}
static void lib_co_server_free_object(zend_object *object)
{
    co_serv *serv_t = (co_serv *) lib_co_server_fetch_object(object);
    if (serv_t->serv)
    {
        php_lib_fci_fcc *handler = serv_t->serv->get_handler();
        if (handler)
        {
            efree(handler);
            serv_t->serv->set_handler(nullptr);
        }
        delete serv_t->serv;
    }
    zend_object_std_dtor(&serv_t->std);
}


/*************定义参数*********************/
ZEND_BEGIN_ARG_INFO_EX(arginfo_lib_co_server_void, 0, 0, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_lib_co_server_construct, 0, 0, 2)
ZEND_ARG_INFO(0, host)
ZEND_ARG_INFO(0, port)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_lib_co_server_handler, 0, 0, 1)
ZEND_ARG_CALLABLE_INFO(0, func, 0)
ZEND_END_ARG_INFO()

/*************定义方法********************/

PHP_METHOD(server_obj,__construct)
{
    co_serv *server_t;
    zend_long port;

    zval *zhost;

    ZEND_PARSE_PARAMETERS_START(2,2)
        Z_PARAM_ZVAL(zhost)
        Z_PARAM_LONG(port)
    ZEND_PARSE_PARAMETERS_END_EX(RETURN_FALSE);

    server_t = (co_serv *)lib_co_server_fetch_object(Z_OBJ_P(getThis()));
    server_t->serv = new Server(Z_STRVAL_P(zhost), port);
    zend_update_property_string(lib_co_server_ce_ptr, getThis(), ZEND_STRL("host"), Z_STRVAL_P(zhost));
    zend_update_property_long(lib_co_server_ce_ptr, getThis(), ZEND_STRL("port"), port);

}

PHP_METHOD(server_obj, start)
{
    co_serv *server_t;

    server_t = (co_serv *)lib_co_server_fetch_object(Z_OBJ_P(getThis()));
    if (server_t->serv->start() == false)
    {
        RETURN_FALSE;
    }
    RETURN_TRUE;
}
PHP_METHOD(server_obj, shutdown)
{
    co_serv *server_t;

    server_t = (co_serv *)lib_co_server_fetch_object(Z_OBJ_P(getThis()));
    if (server_t->serv->shutdown() == false)
    {
        RETURN_FALSE;
    }
    RETURN_TRUE;
}
PHP_METHOD(server_obj, set_handler)
{
    co_serv *server_t;
    php_lib_fci_fcc *handle_fci_fcc;

    handle_fci_fcc = (php_lib_fci_fcc *)emalloc(sizeof(php_lib_fci_fcc));

    ZEND_PARSE_PARAMETERS_START(1, 1)
    Z_PARAM_FUNC(handle_fci_fcc->fci, handle_fci_fcc->fcc)
    ZEND_PARSE_PARAMETERS_END_EX(RETURN_FALSE);

    server_t = (co_serv *)lib_co_server_fetch_object(Z_OBJ_P(getThis()));
    server_t->serv->set_handler(handle_fci_fcc);
}




/*************注册函数***********************/
static const zend_function_entry lib_coroutine_server_coro_methods[] =
        {
                PHP_ME(server_obj, __construct, arginfo_lib_co_server_construct, ZEND_ACC_PUBLIC | ZEND_ACC_CTOR) // ZEND_ACC_CTOR is used to declare that this method is a constructor of this class.
                PHP_ME(server_obj, start, arginfo_lib_co_server_void, ZEND_ACC_PUBLIC)
                PHP_ME(server_obj, shutdown, arginfo_lib_co_server_void, ZEND_ACC_PUBLIC)
                PHP_ME(server_obj, set_handler, arginfo_lib_co_server_handler, ZEND_ACC_PUBLIC)
                PHP_FE_END
        };


void lib_co_server_init(int module_number)
{
    INIT_NS_CLASS_ENTRY(lib_co_server_ce, "Lib", "Coroutine\\Server", lib_coroutine_server_coro_methods);

    memcpy(&lib_co_server_handlers, zend_get_std_object_handlers(), sizeof(zend_object_handlers));
    lib_co_server_ce_ptr = zend_register_internal_class(&lib_co_server_ce TSRMLS_CC); // Registered in the Zend Engine
    SET_CLASS_CUSTOM_OBJECT(lib_co_server, lib_co_server_create_object, lib_co_server_free_object, co_serv, std);

    zend_declare_property_string(lib_co_server_ce_ptr, ZEND_STRL("host"), "", ZEND_ACC_PUBLIC);
    zend_declare_property_long(lib_co_server_ce_ptr, ZEND_STRL("port"), -1, ZEND_ACC_PUBLIC);
    zend_declare_property_long(lib_co_server_ce_ptr, ZEND_STRL("errCode"), 0, ZEND_ACC_PUBLIC);
    zend_declare_property_string(lib_co_server_ce_ptr, ZEND_STRL("errMsg"), "", ZEND_ACC_PUBLIC);
}