//
// Created by root on 2019/12/18.
//

#include "src/automic/automic.h"
#include "php_lib.h"

zend_class_entry lib_automic_ce;
zend_class_entry *lib_automic_ce_ptr


PHP_METHOD(lib_automic,create)
{

}


void lib_automic_util_init(){
    INIT_NS_CLASS_ENTRY(lib_automic_ce,"Lib","Automic",lib_automic_util_methods);
    lib_automic_ce_ptr = zend_register_internal_class(&lib_automic_ce TSRMLS_CC);

//    lib_automic_ce_ptr->serialize = zend_class_serialize_deny;
//    lib_automic_ce_ptr->unserialize = zend_class_unserialize_deny;
    //申明类的属性
    zend_declare_property_null(lib_automic_ce_ptr, ZEND_STRL("slot"), ZEND_ACC_PRIVATE TSRMLS_CC);
    // zend_declare_property_long(lib_automic_ce_ptr, ZEND_STRL("size"), 0, ZEND_ACC_PUBLIC TSRMLS_CC);
}