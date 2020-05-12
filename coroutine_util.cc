#include "lib_coroutine.h"
#include "coroutine.h"
#include "php_lib.h"
#include "lib.h"
using lib::PHPCoroutine;
using lib::Coroutine;
//创建一个无序字典
static std::unordered_map<long, Coroutine *> user_yield_coros;


ZEND_BEGIN_ARG_INFO_EX(arginfo_lib_coroutine_defer,0,0,1)
ZEND_ARG_CALLABLE_INFO(0,func,0)
ZEND_END_ARG_INFO()

//定义yeid接口参数
ZEND_BEGIN_ARG_INFO_EX(arginfo_lib_coroutine_void,0,0,0)
ZEND_END_ARG_INFO()
//定义resume接口参数
ZEND_BEGIN_ARG_INFO_EX(arginfo_lib_coroutine_resume,0,0,1)
    ZEND_ARG_INFO(0,cid)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_lib_coroutine_isExist,0,0,1)
ZEND_ARG_INFO(0,cid)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_lib_coroutine_sleep, 0, 0, 1)
ZEND_ARG_INFO(0, seconds)
ZEND_END_ARG_INFO()

//PHP_METHOD(lib_coroutine_util,create)
//{
PHP_FUNCTION(lib_coroutine_create)
{
    zend_fcall_info fci = empty_fcall_info;
    zend_fcall_info_cache fcc = empty_fcall_info_cache;
    zval result;
    //1 -1 可变参数
    ZEND_PARSE_PARAMETERS_START(1,-1)
        Z_PARAM_FUNC(fci,fcc)
        Z_PARAM_VARIADIC("*",fci.params,fci.param_count)
    ZEND_PARSE_PARAMETERS_END_EX(RETURN_FALSE);
    long cid = PHPCoroutine::create(&fcc,fci.param_count,fci.params);
    RETURN_LONG(cid);
}
PHP_METHOD(lib_coroutine_util,defer)
{
    CallBackParam *defer_call = new CallBackParam;
    ZEND_PARSE_PARAMETERS_START(1,-1)
    Z_PARAM_FUNC(defer_call->fci,defer_call->fcc)
    Z_PARAM_VARIADIC("*",defer_call->fci.params,defer_call->fci.param_count)
    ZEND_PARSE_PARAMETERS_END_EX(RETURN_FALSE);

    PHPCoroutine::defer(defer_call);
}
PHP_METHOD(lib_coroutine_util,yield)
{
    Coroutine *co = Coroutine::get_current();
    user_yield_coros[co->get_cid()] = co;
    co->yield();
    RETURN_TRUE;
}
PHP_METHOD(lib_coroutine_util,resume)
{
    zend_long cid = 0;
    ZEND_PARSE_PARAMETERS_START(1,1)
    Z_PARAM_LONG(cid)
    ZEND_PARSE_PARAMETERS_END_EX(RETURN_FALSE);

    auto coroutine_iterator = user_yield_coros.find(cid);
    if (coroutine_iterator == user_yield_coros.end())
    {
        php_error_docref(NULL, E_WARNING, "resume error");
        RETURN_FALSE;
    }
    Coroutine * co = coroutine_iterator->second;
    user_yield_coros.erase(cid);
    co->resume();
    RETURN_TRUE;
}

PHP_METHOD(lib_coroutine_util,getCid)
{
    Coroutine *co = Coroutine::get_current();
    if(co == nullptr){
        RETURN_LONG(-1);
    }

    RETURN_LONG(co->get_cid());
}

PHP_METHOD(lib_coroutine_util,isExist)
{

    zend_long cid = 0;
    bool is_exist;

    ZEND_PARSE_PARAMETERS_START(1,1)
    Z_PARAM_LONG(cid)
    ZEND_PARSE_PARAMETERS_END_EX(RETURN_FALSE);

    auto coroutine_iterator = user_yield_coros.find(cid);
    is_exist = (coroutine_iterator != user_yield_coros.end());
    RETURN_BOOL(is_exist);
}
PHP_METHOD(lib_coroutine_util,sleep)
{
    double seconds;

    ZEND_PARSE_PARAMETERS_START(1,1)
        Z_PARAM_DOUBLE(seconds)
    ZEND_PARSE_PARAMETERS_END_EX(RETURN_FALSE);
    if(UNEXPECTED(seconds < 0.001)){
        php_error_docref(NULL,E_WARNING,"timer must be greater than or equal to 0.001");
        RETURN_FALSE;
    }
    PHPCoroutine::sleep(seconds);
    RETURN_TRUE;
}
PHP_FUNCTION(lib_event_init)
{
    int ret;
    ret = event_init();
    if (ret < 0)
    {
        RETURN_FALSE;
    }
    RETURN_TRUE;
}

PHP_FUNCTION(lib_event_wait)
{
    int ret;
    ret = event_wait();
    if (ret < 0)
    {
        RETURN_FALSE;
    }
    RETURN_TRUE;
}
const zend_function_entry lib_coroutine_util_methods[] =
{
    ZEND_FENTRY(create,ZEND_FN(lib_coroutine_create),arginfo_lib_coroutine_create,ZEND_ACC_PUBLIC | ZEND_ACC_STATIC)
   // PHP_ME(lib_coroutine_util,create,arginfo_lib_coroutine_create,ZEND_ACC_PUBLIC | ZEND_ACC_STATIC)
    PHP_ME(lib_coroutine_util,defer,arginfo_lib_coroutine_defer,ZEND_ACC_PUBLIC | ZEND_ACC_STATIC)
    PHP_ME(lib_coroutine_util,yield,arginfo_lib_coroutine_void,ZEND_ACC_PUBLIC | ZEND_ACC_STATIC)
    PHP_ME(lib_coroutine_util,resume,arginfo_lib_coroutine_resume,ZEND_ACC_PUBLIC | ZEND_ACC_STATIC)
    PHP_ME(lib_coroutine_util,isExist,arginfo_lib_coroutine_isExist,ZEND_ACC_PUBLIC | ZEND_ACC_STATIC)
    PHP_ME(lib_coroutine_util,getCid,arginfo_lib_coroutine_void,ZEND_ACC_PUBLIC | ZEND_ACC_STATIC)
    PHP_ME(lib_coroutine_util,sleep,arginfo_lib_coroutine_sleep,ZEND_ACC_PUBLIC | ZEND_ACC_STATIC)
    PHP_FE_END
};

zend_class_entry lib_coroutine_ce;
zend_class_entry *lib_coroutine_ce_ptr;

//???????????
void lib_coroutine_util_init()
{
    PHPCoroutine::init();

    INIT_NS_CLASS_ENTRY(lib_coroutine_ce,"Lib","Coroutine",lib_coroutine_util_methods);
    lib_coroutine_ce_ptr = zend_register_internal_class(&lib_coroutine_ce TSRMLS_CC);
    //给类增加短名机制
    zend_register_class_alias("Cco", lib_coroutine_ce_ptr);
}