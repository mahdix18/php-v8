/*
 * This file is part of the pinepain/php-v8 PHP extension.
 *
 * Copyright (c) 2015-2017 Bogdan Padalko <pinepain@gmail.com>
 *
 * Licensed under the MIT license: http://opensource.org/licenses/MIT
 *
 * For the full copyright and license information, please view the
 * LICENSE file that was distributed with this source or visit
 * http://opensource.org/licenses/MIT
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php_v8_number.h"
#include "php_v8_primitive.h"
#include "php_v8_value.h"
#include "php_v8.h"

zend_class_entry *php_v8_number_class_entry;
#define this_ce php_v8_number_class_entry


static PHP_METHOD(V8Number, __construct) {
    zval *php_v8_isolate_zv;

    double value = 0;

    if (zend_parse_parameters(ZEND_NUM_ARGS(), "od", &php_v8_isolate_zv, &value) == FAILURE) {
        return;
    }

    PHP_V8_VALUE_CONSTRUCT(getThis(), php_v8_isolate_zv, php_v8_isolate, php_v8_value);

    v8::Local<v8::Number> number_tpl = v8::Number::New(isolate, value);

    PHP_V8_THROW_VALUE_EXCEPTION_WHEN_EMPTY(number_tpl, "Failed to create Number value");

    php_v8_value->persistent->Reset(isolate, number_tpl);
}


static PHP_METHOD(V8Number, Value) {
    if (zend_parse_parameters_none() == FAILURE) {
        return;
    }

    PHP_V8_VALUE_FETCH_WITH_CHECK(getThis(), php_v8_value);
    PHP_V8_ENTER_ISOLATE(php_v8_value->php_v8_isolate);

    v8::Local<v8::Number> local_number = php_v8_value_get_local_as<v8::Number>(php_v8_value);

    RETVAL_DOUBLE(local_number->Value());
}


ZEND_BEGIN_ARG_INFO_EX(arginfo_v8_number___construct, ZEND_SEND_BY_VAL, ZEND_RETURN_VALUE, 2)
                ZEND_ARG_OBJ_INFO(0, isolate, V8\\Isolate, 0)
                ZEND_ARG_TYPE_INFO(0, value, IS_DOUBLE, 0)
ZEND_END_ARG_INFO()

// no strict typing while we'll inherit this class
ZEND_BEGIN_ARG_INFO_EX(arginfo_v8_number_Value, ZEND_SEND_BY_VAL, ZEND_RETURN_VALUE, 0)
ZEND_END_ARG_INFO()


static const zend_function_entry php_v8_number_methods[] = {
        PHP_ME(V8Number, __construct, arginfo_v8_number___construct, ZEND_ACC_PUBLIC | ZEND_ACC_CTOR)
        PHP_ME(V8Number, Value, arginfo_v8_number_Value, ZEND_ACC_PUBLIC)
        PHP_FE_END
};


PHP_MINIT_FUNCTION(php_v8_number) {
    zend_class_entry ce;
    INIT_NS_CLASS_ENTRY(ce, PHP_V8_NS, "NumberValue", php_v8_number_methods);
    this_ce = zend_register_internal_class_ex(&ce, php_v8_primitive_class_entry);

    return SUCCESS;
}
