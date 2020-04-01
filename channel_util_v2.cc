#include "coroutine_channel.h"
#include "php_lib.h"
#include "lib_class.h"
using lib::coroutine::Channel;

//php在操作对象时 底层会调用的函数
INIT_CLASS_HANDLER(co_chan)

//自定义php对象
struct co_chan
{
    Channel *chan;
    zend_object std;

    FETCH_METHOD(co_chan)
    CREATE_METHOD(co_chan)
    INIT_METHOD(co_chan,Channel,chan)

    //define free method
    FREE_METHOD_START(co_chan,chan)
        Channel *chan = obj->chan;  
        while(!chan->empty()){
            zval *data;
            data = (zval *)chan->pop_data();
            zval_ptr_dtor(data);
            efree(data);
        }
        delete chan;
    FREE_MEHTOD_END
};

ZEND_BEGIN_ARG_INFO_EX(arginfo_lib_co_channel_construct, 0, 0, 0)
ZEND_ARG_INFO(0, capacity)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_lib_co_channel_push, 0, 0, 1)
ZEND_ARG_INFO(0, data)
ZEND_ARG_INFO(0, timeout)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_lib_co_channel_pop, 0, 0, 0)
ZEND_ARG_INFO(0, timeout)
ZEND_END_ARG_INFO()


static PHP_METHOD(lib_co_channel, __construct)
{

    co_chan *chan_t;
    zend_long capacity = 1;

    ZEND_PARSE_PARAMETERS_START_EX(ZEND_PARSE_PARAMS_THROW, 0, 1)
    Z_PARAM_OPTIONAL
            Z_PARAM_LONG(capacity)
    ZEND_PARSE_PARAMETERS_END_EX(RETURN_FALSE);

    if (capacity <= 0)
    {
        capacity = 1;
    }

    chan_t = co_chan::fetch_object(GET_THIS_OBJECT);
    chan_t->chan = new Channel(capacity);
    zend_update_property_long(THIS_PTR(co_chan), getThis(), ZEND_STRL("capacity"), capacity);
}

static PHP_METHOD(lib_co_channel, push)
{
    co_chan *chan_t;
    Channel *chan;
    zval *zdata;
    double timeout = -1;

    ZEND_PARSE_PARAMETERS_START(1, 2)
    Z_PARAM_ZVAL(zdata)
    Z_PARAM_OPTIONAL
            Z_PARAM_DOUBLE(timeout)
    ZEND_PARSE_PARAMETERS_END_EX(RETURN_FALSE);

    chan_t = co_chan::fetch_object(GET_THIS_OBJECT);
    chan  = chan_t->chan;

    //防止字符串被销毁了，增加引用计数
    Z_TRY_ADDREF_P(zdata);
    zdata = lib_zval_dup(zdata);
    if (!chan->push(zdata, timeout))
    {
        //删除引用，回收字符串
        Z_TRY_DELREF_P(zdata);
        efree(zdata);
        RETURN_FALSE;
    }

    RETURN_TRUE;
}

static PHP_METHOD(lib_co_channel, pop)
{
    co_chan *chan_t;
    Channel *chan;
    double timeout = -1;

    ZEND_PARSE_PARAMETERS_START(0, 1)
    Z_PARAM_OPTIONAL
            Z_PARAM_DOUBLE(timeout)
    ZEND_PARSE_PARAMETERS_END_EX(RETURN_FALSE);

    chan_t = co_chan::fetch_object(GET_THIS_OBJECT);
    chan  = chan_t->chan;
    zval *zdata = (zval *)chan->pop(timeout);
    if (!zdata)
    {
        RETURN_FALSE;
    }
    //返回用户push 的数据
    RETVAL_ZVAL(zdata, 0, 0);
    //释放拷贝出来的内存
    efree(zdata);
}

static const zend_function_entry lib_co_channel_methods[] =
        {
                PHP_ME(lib_co_channel, __construct, arginfo_lib_co_channel_construct, ZEND_ACC_PUBLIC | ZEND_ACC_CTOR) // ZEND_ACC_CTOR is used to declare that this method is a constructor of this class.
                PHP_ME(lib_co_channel, push, arginfo_lib_co_channel_push, ZEND_ACC_PUBLIC | ZEND_ACC_CTOR) // ZEND_ACC_CTOR is used to declare that this method is a constructor of this class.
                PHP_ME(lib_co_channel, pop, arginfo_lib_co_channel_pop, ZEND_ACC_PUBLIC | ZEND_ACC_CTOR) // ZEND_ACC_CTOR is used to declare that this method is a constructor of this class.
                PHP_FE_END
        };

void lib_channel_init() {


    INIT_NS_CLASS_ENTRY(THIS_CE(co_chan), "Lib", "Coroutine\\Channel", lib_co_channel_methods);
    THIS_PTR(co_chan) = zend_register_internal_class(&THIS_CE(co_chan)    TSRMLS_CC); // Registered in the Zend Engine

    memcpy(&THIS_HANDLER(co_chan), zend_get_std_object_handlers(), sizeof(zend_object_handlers));

    REGISTER_CUSTOM_OBJECT(co_chan);
    zend_declare_property_long(THIS_PTR(co_chan), ZEND_STRL("capacity"), 1, ZEND_ACC_PUBLIC);
}
