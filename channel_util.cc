#include "coroutine_channel.h"
#include "php_lib.h"
using lib::coroutine::Channel;

ZEND_BEGIN_ARG_INFO_EX(arginfo_lib_coro_channel_construct, 0, 0, 0)
ZEND_ARG_INFO(0, capacity)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_lib_coro_channel_push, 0, 0, 1)
ZEND_ARG_INFO(0, data)
ZEND_ARG_INFO(0, timeout)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_lib_coro_channel_pop, 0, 0, 0)
ZEND_ARG_INFO(0, timeout)
ZEND_END_ARG_INFO()
/**
 * Define zend class entry
 */
zend_class_entry lib_coro_channel_ce;
zend_class_entry *lib_coro_channel_ce_ptr;


static PHP_METHOD(lib_coro_channel, __construct)
{
    zval zchan;
    zend_long capacity = 1;

    ZEND_PARSE_PARAMETERS_START_EX(ZEND_PARSE_PARAMS_THROW, 0, 1)
        Z_PARAM_OPTIONAL
        Z_PARAM_LONG(capacity)
    ZEND_PARSE_PARAMETERS_END_EX(RETURN_FALSE);

    if (capacity <= 0)
    {
        capacity = 1;
    }

    zend_update_property_long(lib_coro_channel_ce_ptr, getThis(), ZEND_STRL("capacity"), capacity);

    Channel *chan = new Channel(capacity);
    //将c++chan对象，转换为 zval指针
    ZVAL_PTR(&zchan, chan);
    zend_update_property(lib_coro_channel_ce_ptr, getThis(), ZEND_STRL("zchan"), &zchan);
}

static PHP_METHOD(lib_coro_channel, push)
{
    zval *zchan;
    Channel *chan;
    zval *zdata;
    double timeout = -1;

    ZEND_PARSE_PARAMETERS_START(1, 2)
        Z_PARAM_ZVAL(zdata)
        Z_PARAM_OPTIONAL
        Z_PARAM_DOUBLE(timeout)
    ZEND_PARSE_PARAMETERS_END_EX(RETURN_FALSE);

    zchan = lib_zend_read_property(lib_coro_channel_ce_ptr, getThis(), ZEND_STRL("zchan"), 0);
    //将zval指针 转换为 channel 对象
    chan = (Channel *)Z_PTR_P(zchan);

    if (!chan->push(zdata, timeout))
    {
        RETURN_FALSE;
    }

    RETURN_TRUE;
}

static PHP_METHOD(lib_coro_channel, pop)
{
    zval *zchan;
    Channel *chan;
    double timeout = -1;

    ZEND_PARSE_PARAMETERS_START(0, 1)
        Z_PARAM_OPTIONAL
        Z_PARAM_DOUBLE(timeout)
    ZEND_PARSE_PARAMETERS_END_EX(RETURN_FALSE);

    zchan = lib_zend_read_property(lib_coro_channel_ce_ptr, getThis(), ZEND_STRL("zchan"), 0);
    chan = (Channel *)Z_PTR_P(zchan);
    zval *zdata = (zval *)chan->pop(timeout);
    if (!zdata)
    {
        RETURN_FALSE;
    }
    //返回用户push 的数据
    RETVAL_ZVAL(zdata, 0, 0);
}

static const zend_function_entry lib_coro_channel_methods[] =
        {
                PHP_ME(lib_coro_channel, __construct, arginfo_lib_coro_channel_construct, ZEND_ACC_PUBLIC | ZEND_ACC_CTOR) // ZEND_ACC_CTOR is used to declare that this method is a constructor of this class.
                PHP_ME(lib_coro_channel, push, arginfo_lib_coro_channel_push, ZEND_ACC_PUBLIC | ZEND_ACC_CTOR) // ZEND_ACC_CTOR is used to declare that this method is a constructor of this class.
                PHP_ME(lib_coro_channel, pop, arginfo_lib_coro_channel_pop, ZEND_ACC_PUBLIC | ZEND_ACC_CTOR) // ZEND_ACC_CTOR is used to declare that this method is a constructor of this class.
                PHP_FE_END
        };

void lib_channel_init()
{
    zval zchan;


    INIT_NS_CLASS_ENTRY(lib_coro_channel_ce, "Lib", "Coroutine\\Channel", lib_coro_channel_methods);
    lib_coro_channel_ce_ptr = zend_register_internal_class(&lib_coro_channel_ce TSRMLS_CC); // Registered in the Zend Engine

    zend_declare_property(lib_coro_channel_ce_ptr, ZEND_STRL("zchan"), &zchan, ZEND_ACC_PUBLIC);
    zend_declare_property_long(lib_coro_channel_ce_ptr, ZEND_STRL("capacity"), 1, ZEND_ACC_PUBLIC);
}