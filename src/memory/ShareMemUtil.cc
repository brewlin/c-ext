#include "php_lib.h"
#include "lib_shamem.h"
u_char *addr;

zend_class_entry lib_shamem_ce;
zend_class_entry *lib_shamem_ce_ptr;

//创建一块共享内存 传递为普通参数int size
ZEND_BEGIN_ARG_INFO_EX(arginfo_init,0,0,1)
ZEND_ARG_INFO(0,size)
ZEND_END_ARG_INFO()

//声明对象的初始化方法方法
static PHP_METHOD(shamem_obj,__construct);
static PHP_METHOD(shamem_obj,add);
static PHP_METHOD(shamem_obj,get);


PHP_METHOD(shamem_obj,__construct)
{

    long initSize = 0;
    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "l", &initSize) == FAILURE)
    {
        return;
    }
    zend_update_property_long(lib_shamem_ce_ptr,getThis(), ZEND_STRL("size"), initSize TSRMLS_CC);

    shm_t shm;
    shm.size = (size_t)initSize ;
    shm_alloc(&shm);
    addr = (u_char *)shm.addr;

    php_printf("sharmem construct size:%ld",initSize);


}
PHP_METHOD(shamem_obj,add)
{

}
PHP_METHOD(shamem_obj,get)
{


}

const zend_function_entry lib_shamem_util_methods[] =
        {
//                PHP_ME(lib_shamem_util,__construct,arginfo_lib_shamem_create,ZEND_ACC_PUBLIC | ZEND_ACC_STATIC)
                PHP_ME(shamem_obj,__construct,arginfo_init,ZEND_ACC_PUBLIC)
                PHP_ME(shamem_obj,add,NULL,ZEND_ACC_PUBLIC)
                PHP_ME(shamem_obj,get,NULL,ZEND_ACC_PUBLIC)
                PHP_FE_END
        };



//注册该类
void lib_sharemem_util_init()
{
    INIT_NS_CLASS_ENTRY(lib_shamem_ce,"Lib","SharMem",lib_shamem_util_methods);
    lib_shamem_ce_ptr = zend_register_internal_class(&lib_shamem_ce TSRMLS_CC);

//    lib_shamem_ce_ptr->serialize = zend_class_serialize_deny;
//    lib_shamem_ce_ptr->unserialize = zend_class_unserialize_deny;
    //申明类的属性
    zend_declare_property_null(lib_shamem_ce_ptr, ZEND_STRL("addr"), ZEND_ACC_PRIVATE TSRMLS_CC);
    zend_declare_property_long(lib_shamem_ce_ptr, ZEND_STRL("size"), 0, ZEND_ACC_PUBLIC TSRMLS_CC);

}