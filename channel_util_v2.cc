#include "coroutine_channel.h"
#include "php_lib.h"
using lib::coroutine::Channel;

//php在操作对象时 底层会调用的函数
static zend_object_handlers lib_co_channel_handlers;

//自定义php对象
typedef struct
{
    Channel *chan;
    zend_object std;
}co_chan;

//通过php对象 定位chan指针
static co_chan *lib_co_channel_fetch_object(zend_object *obj)
{
    //通过offset 计算上一个属性的 内存地址 获得 chan的内存地址
    return (co_chan *)((char *)obj - lib_co_channel_handlers.offset);
}
//创建php对象
static zend_object *lib_co_channel_create_object(zend_class_entry *ce)
{
    co_chan *chan_t = (co_chan *)ecalloc(1, sizeof(co_chan) + zend_object_properties_size(ce));
    zend_object_std_init(&chan_t->std, ce);
    object_properties_init(&chan_t->std, ce);
    chan_t->std.handlers = &lib_co_channel_handlers;
    return &chan_t->std;
}
//释放php对象
static void lib_co_channel_free_object(zend_object *object)
{
    co_chan *chan_t = (co_chan *)lib_co_channel_fetch_object(object);
    Channel *chan = chan_t->chan;

    if(chan)
    {
        while(!chan->empty()){
            zval *data;
            data = (zval *)chan->pop_data();
            zval_ptr_dtor(data);
            efree(data);
        }
        delete chan;
    }
    zend_object_std_dtor(&chan_t->std);
}


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
/**
 * Define zend class entry
 */
zend_class_entry lib_co_channel_ce;
zend_class_entry *lib_co_channel_ce_ptr;


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

    chan_t = (co_chan *)lib_co_channel_fetch_object(Z_OBJ_P(getThis()));
    chan_t->chan = new Channel(capacity);
    zend_update_property_long(lib_co_channel_ce_ptr, getThis(), ZEND_STRL("capacity"), capacity);
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

    chan_t = (co_chan *)lib_co_channel_fetch_object(Z_OBJ_P(getThis()));
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

    chan_t = (co_chan *)lib_co_channel_fetch_object(Z_OBJ_P(getThis()));
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


    INIT_NS_CLASS_ENTRY(lib_co_channel_ce, "Lib", "Coroutine\\Channel", lib_co_channel_methods);
    lib_co_channel_ce_ptr = zend_register_internal_class(&lib_co_channel_ce    TSRMLS_CC); // Registered in the Zend Engine

    memcpy(&lib_co_channel_handlers, zend_get_std_object_handlers(), sizeof(zend_object_handlers));
    ST_SET_CLASS_CUSTOM_OBJECT(lib_co_channel, lib_co_channel_create_object, lib_co_channel_free_object, co_chan,
                               std);
    zend_declare_property_long(lib_co_channel_ce_ptr, ZEND_STRL("capacity"), 1, ZEND_ACC_PUBLIC);
}
