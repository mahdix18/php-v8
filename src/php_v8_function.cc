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

#include "php_v8_script_origin.h"
#include "php_v8_function.h"
#include "php_v8_value.h"
#include "php_v8_string.h"
#include "php_v8_object.h"
#include "php_v8_context.h"
#include "php_v8_enums.h"
#include "php_v8.h"

zend_class_entry *php_v8_function_class_entry;
#define this_ce php_v8_function_class_entry


bool php_v8_function_unpack_args(zval *arguments_zv, int arg_position, v8::Isolate *isolate, int *argc, v8::Local<v8::Value> **argv) {
    if (NULL == arguments_zv || zend_hash_num_elements(Z_ARRVAL_P(arguments_zv)) < 1) {
        return true;
    }

    php_v8_value_t *php_v8_tmp_data;

    int i = 0;
    bool has_error = false;

    HashTable *myht;
    zval *pzval;

    *argc = zend_hash_num_elements(Z_ARRVAL_P(arguments_zv));
    *argv = (v8::Local<v8::Value> *) ecalloc(static_cast<size_t>(*argc), sizeof(*argv));

    myht = Z_ARRVAL_P(arguments_zv);

    char *exception_message;

    zend_string *ce_name = zend_get_executed_scope()->name;

    ZEND_HASH_FOREACH_VAL(myht, pzval) {
        if (Z_TYPE_P(pzval) != IS_OBJECT) {
            zend_throw_error(zend_ce_type_error,
                             "Argument %d passed to %s::%s() must be an array of \\V8\\Value objects, %s given at %d offset",
                             arg_position, ZSTR_VAL(ce_name), get_active_function_name(),
                             zend_zval_type_name(pzval), i);

            has_error = true;
            break;
        }

        if (!instanceof_function(Z_OBJCE_P(pzval), php_v8_value_class_entry)) {
            zend_throw_error(zend_ce_type_error,
                             "Argument %d passed to %s::%s() must be an array of \\V8\\Value objects, instance of %s given at %d offset",
                             arg_position, ZSTR_VAL(ce_name), get_active_function_name(),
                             ZSTR_VAL(Z_OBJCE_P(pzval)->name), i);

            has_error = true;
            break;
        }

        php_v8_tmp_data = PHP_V8_VALUE_FETCH(pzval);

        // Check for emptiness may be considered redundant while we may catch the fact that value was not properly
        // constructed by checking isolates mismatch, but this check serves for user-friendly purposes to throw
        // less confusing exception message
        if (NULL == php_v8_tmp_data->persistent || php_v8_tmp_data->persistent->IsEmpty()) {
            spprintf(&exception_message, 0, PHP_V8_EMPTY_VALUE_MSG ": argument %d passed to %s::%s() at %d offset",
                     arg_position, ZSTR_VAL(ce_name), get_active_function_name(), i);

            PHP_V8_THROW_EXCEPTION(exception_message);

            efree(exception_message);
            has_error = true;
            break;
        }

        if (NULL == php_v8_tmp_data->php_v8_isolate || isolate != php_v8_tmp_data->php_v8_isolate->isolate) {
            spprintf(&exception_message, 0,
                     PHP_V8_ISOLATES_MISMATCH_MSG ": argument %d passed to %s::%s() at %d offset",
                     arg_position, ZSTR_VAL(ce_name), get_active_function_name(), i);

            PHP_V8_THROW_EXCEPTION(exception_message);

            efree(exception_message);
            has_error = true;
            break;
        }

        (*argv)[i++] = php_v8_value_get_local(php_v8_tmp_data);
    } ZEND_HASH_FOREACH_END();

    if (has_error) {
        efree(*argv);
        *argv = NULL;
        *argc = 0;

        return false;
    }

    return true;
}

bool php_v8_function_unpack_string_args(zval* arguments_zv, int arg_position, v8::Isolate *isolate, int *argc, v8::Local<v8::String> **argv) {
    if (NULL == arguments_zv || zend_hash_num_elements(Z_ARRVAL_P(arguments_zv)) < 1) {
        return true;
    }

    php_v8_value_t *php_v8_tmp_data;

    int i = 0;
    bool has_error = false;

    HashTable *myht;
    zval *pzval;

    *argc = zend_hash_num_elements(Z_ARRVAL_P(arguments_zv));
    *argv = (v8::Local<v8::String> *) ecalloc(static_cast<size_t>(*argc), sizeof(*argv));

    myht = Z_ARRVAL_P(arguments_zv);

    char *exception_message;

    zend_string *ce_name = zend_get_executed_scope()->name;

    ZEND_HASH_FOREACH_VAL(myht, pzval) {
                if (Z_TYPE_P(pzval) != IS_OBJECT) {
                    zend_throw_error(zend_ce_type_error,
                                     "Argument %d passed to %s::%s() must be an array of \\V8\\StringValue objects, %s given at %d offset",
                                     arg_position, ZSTR_VAL(ce_name), get_active_function_name(),
                                     zend_zval_type_name(pzval), i);

                    has_error = true;
                    break;
                }

                if (!instanceof_function(Z_OBJCE_P(pzval), php_v8_string_class_entry)) {
                    zend_throw_error(zend_ce_type_error,
                                     "Argument %d passed to %s::%s() must be an array of \\V8\\StringValue, instance of %s given at %d offset",
                                     arg_position, ZSTR_VAL(ce_name), get_active_function_name(),
                                     ZSTR_VAL(Z_OBJCE_P(pzval)->name), i);

                    has_error = true;
                    break;
                }

                php_v8_tmp_data = PHP_V8_VALUE_FETCH(pzval);

                // Check for emptiness may be considered redundant while we may catch the fact that value was not properly
                // constructed by checking isolates mismatch, but this check serves for user-friendly purposes to throw
                // less confusing exception message
                if (NULL == php_v8_tmp_data->persistent || php_v8_tmp_data->persistent->IsEmpty()) {
                    spprintf(&exception_message, 0, PHP_V8_EMPTY_VALUE_MSG ": argument %d passed to %s::%s() at %d offset",
                             arg_position, ZSTR_VAL(ce_name), get_active_function_name(), i);

                    PHP_V8_THROW_EXCEPTION(exception_message);

                    efree(exception_message);
                    has_error = true;
                    break;
                }

                if (NULL == php_v8_tmp_data->php_v8_isolate || isolate != php_v8_tmp_data->php_v8_isolate->isolate) {
                    spprintf(&exception_message, 0,
                             PHP_V8_ISOLATES_MISMATCH_MSG ": argument %d passed to %s::%s() at %d offset",
                             arg_position, ZSTR_VAL(ce_name), get_active_function_name(), i);

                    PHP_V8_THROW_EXCEPTION(exception_message);

                    efree(exception_message);
                    has_error = true;
                    break;
                }

                (*argv)[i++] = php_v8_value_get_local_as<v8::String>(php_v8_tmp_data);
            } ZEND_HASH_FOREACH_END();

    if (has_error) {
        efree(*argv);
        *argv = NULL;
        *argc = 0;

        return false;
    }

    return true;
}

bool php_v8_function_unpack_object_args(zval* arguments_zv, int arg_position, v8::Isolate *isolate, int *argc, v8::Local<v8::Object> **argv) {
    if (NULL == arguments_zv || zend_hash_num_elements(Z_ARRVAL_P(arguments_zv)) < 1) {
        return true;
    }

    php_v8_value_t *php_v8_tmp_data;

    int i = 0;
    bool has_error = false;

    HashTable *myht;
    zval *pzval;

    *argc = zend_hash_num_elements(Z_ARRVAL_P(arguments_zv));
    *argv = (v8::Local<v8::Object> *) ecalloc(static_cast<size_t>(*argc), sizeof(*argv));

    myht = Z_ARRVAL_P(arguments_zv);

    char *exception_message;

    zend_string *ce_name = zend_get_executed_scope()->name;

    ZEND_HASH_FOREACH_VAL(myht, pzval) {
                if (Z_TYPE_P(pzval) != IS_OBJECT) {
                    zend_throw_error(zend_ce_type_error,
                                     "Argument %d passed to %s::%s() must be an array of \\V8\\ObjectValue objects, %s given at %d offset",
                                     arg_position, ZSTR_VAL(ce_name), get_active_function_name(),
                                     zend_zval_type_name(pzval), i);

                    has_error = true;
                    break;
                }

                if (!instanceof_function(Z_OBJCE_P(pzval), php_v8_object_class_entry)) {
                    zend_throw_error(zend_ce_type_error,
                                     "Argument %d passed to %s::%s() must be an array of \\V8\\ObjectValue, instance of %s given at %d offset",
                                     arg_position, ZSTR_VAL(ce_name), get_active_function_name(),
                                     ZSTR_VAL(Z_OBJCE_P(pzval)->name), i);

                    has_error = true;
                    break;
                }

                php_v8_tmp_data = PHP_V8_VALUE_FETCH(pzval);

                // Check for emptiness may be considered redundant while we may catch the fact that value was not properly
                // constructed by checking isolates mismatch, but this check serves for user-friendly purposes to throw
                // less confusing exception message
                if (NULL == php_v8_tmp_data->persistent || php_v8_tmp_data->persistent->IsEmpty()) {
                    spprintf(&exception_message, 0, PHP_V8_EMPTY_VALUE_MSG ": argument %d passed to %s::%s() at %d offset",
                             arg_position, ZSTR_VAL(ce_name), get_active_function_name(), i);

                    PHP_V8_THROW_EXCEPTION(exception_message);

                    efree(exception_message);
                    has_error = true;
                    break;
                }

                if (NULL == php_v8_tmp_data->php_v8_isolate || isolate != php_v8_tmp_data->php_v8_isolate->isolate) {
                    spprintf(&exception_message, 0,
                             PHP_V8_ISOLATES_MISMATCH_MSG ": argument %d passed to %s::%s() at %d offset",
                             arg_position, ZSTR_VAL(ce_name), get_active_function_name(), i);

                    PHP_V8_THROW_EXCEPTION(exception_message);

                    efree(exception_message);
                    has_error = true;
                    break;
                }

                (*argv)[i++] = php_v8_value_get_local_as<v8::Object>(php_v8_tmp_data);
            } ZEND_HASH_FOREACH_END();

    if (has_error) {
        efree(*argv);
        *argv = NULL;
        *argc = 0;

        return false;
    }

    return true;
}


static PHP_METHOD(V8Function, __construct) {
    zval rv;
    zval *php_v8_context_zv;

    zend_fcall_info fci = empty_fcall_info;
    zend_fcall_info_cache fci_cache = empty_fcall_info_cache;

    zend_long length = 0;
    zend_long behavior = static_cast<zend_long>(v8::ConstructorBehavior::kAllow);

    v8::FunctionCallback callback = 0;
    v8::Local<v8::External> data;

    if (zend_parse_parameters(ZEND_NUM_ARGS(), "of|ll", &php_v8_context_zv, &fci, &fci_cache, &length, &behavior) == FAILURE) {
        return;
    }

    behavior = behavior ? behavior & PHP_V8_CONSTRUCTOR_BEHAVIOR_FLAGS : behavior;

    PHP_V8_CHECK_FUNCTION_LENGTH_RANGE(length, "Length is out of range");

    PHP_V8_VALUE_FETCH_INTO(getThis(), php_v8_value);

    PHP_V8_CONTEXT_FETCH_WITH_CHECK(php_v8_context_zv, php_v8_context);
    PHP_V8_OBJECT_STORE_CONTEXT(getThis(), php_v8_context_zv);

    PHP_V8_VALUE_STORE_ISOLATE(getThis(), PHP_V8_CONTEXT_READ_ISOLATE(php_v8_context_zv));
    PHP_V8_STORE_POINTER_TO_CONTEXT(php_v8_value, php_v8_context);
    PHP_V8_COPY_POINTER_TO_ISOLATE(php_v8_value, php_v8_context);

    PHP_V8_ENTER_STORED_ISOLATE(php_v8_context);
    PHP_V8_ENTER_CONTEXT(php_v8_context);

    if (fci.size) {
        phpv8::CallbacksBucket *bucket = php_v8_value->persistent_data->bucket("callback");
        data = v8::External::New(isolate, bucket);

        bucket->add(phpv8::CallbacksBucket::Index::Getter, fci, fci_cache);

        callback = php_v8_callback_function;
    }

    v8::MaybeLocal<v8::Function> maybe_local_function = v8::Function::New(context,
                                                                          callback,
                                                                          data,
                                                                          static_cast<int>(length),
                                                                          static_cast<v8::ConstructorBehavior>(behavior));

    if (maybe_local_function.IsEmpty()) {
        PHP_V8_THROW_EXCEPTION("Failed to create Function value");
        return;
    }

    v8::Local<v8::Function> local_function = maybe_local_function.ToLocalChecked();

    php_v8_object_store_self_ptr(php_v8_value, local_function);

    php_v8_value->persistent->Reset(isolate, local_function);
}

static PHP_METHOD(V8Function, NewInstance) {
    zval *php_v8_context_zv;
    zval *arguments_zv = NULL;

    int argc = 0;
    v8::Local<v8::Value> *argv = NULL;


    if (zend_parse_parameters(ZEND_NUM_ARGS(), "o|a", &php_v8_context_zv, &arguments_zv) == FAILURE) {
        return;
    }

    PHP_V8_VALUE_FETCH_WITH_CHECK(getThis(), php_v8_value);
    PHP_V8_CONTEXT_FETCH_WITH_CHECK(php_v8_context_zv, php_v8_context);

    PHP_V8_DATA_ISOLATES_CHECK(php_v8_value, php_v8_context);

    PHP_V8_ENTER_STORED_ISOLATE(php_v8_context);
    PHP_V8_ENTER_CONTEXT(php_v8_context);

    if (!php_v8_function_unpack_args(arguments_zv, 2, isolate, &argc, &argv)) {
        return;
    }

    v8::Local<v8::Function> local_function = php_v8_value_get_local_as<v8::Function>(php_v8_value);

    PHP_V8_TRY_CATCH(isolate);
    PHP_V8_INIT_ISOLATE_LIMITS_ON_CONTEXT(php_v8_context);

    v8::MaybeLocal<v8::Object> maybe_local_obj = local_function->NewInstance(context, argc, argv);

    if (argv) {
        efree(argv);
    }

    PHP_V8_MAYBE_CATCH(php_v8_context, try_catch);
    PHP_V8_THROW_VALUE_EXCEPTION_WHEN_EMPTY(maybe_local_obj, "Failed to create instance");

    v8::Local<v8::Object> local_obj = maybe_local_obj.ToLocalChecked();

    php_v8_get_or_create_value(return_value, local_obj, php_v8_value->php_v8_isolate);
}

static PHP_METHOD(V8Function, Call) {
    zval *php_v8_context_zv;
    zval *php_v8_recv_zv;
    zval *arguments_zv = NULL;

    int argc = 0;
    v8::Local<v8::Value> *argv = NULL;

    if (zend_parse_parameters(ZEND_NUM_ARGS(), "oo|a", &php_v8_context_zv, &php_v8_recv_zv, &arguments_zv) == FAILURE) {
        return;
    }

    PHP_V8_VALUE_FETCH_WITH_CHECK(getThis(), php_v8_value);
    PHP_V8_VALUE_FETCH_WITH_CHECK(php_v8_recv_zv, php_v8_value_recv);
    PHP_V8_CONTEXT_FETCH_WITH_CHECK(php_v8_context_zv, php_v8_context);

    PHP_V8_DATA_ISOLATES_CHECK(php_v8_value, php_v8_value_recv)
    PHP_V8_DATA_ISOLATES_CHECK(php_v8_value, php_v8_context)

    PHP_V8_ENTER_STORED_ISOLATE(php_v8_context);
    PHP_V8_ENTER_CONTEXT(php_v8_context);

    if (!php_v8_function_unpack_args(arguments_zv, 3, isolate, &argc, &argv)) {
        return;
    }

    v8::Local<v8::Value> local_recv = php_v8_value_get_local(php_v8_value_recv);
    v8::Local<v8::Function> local_function = php_v8_value_get_local_as<v8::Function>(php_v8_value);

    PHP_V8_TRY_CATCH(isolate);
    PHP_V8_INIT_ISOLATE_LIMITS_ON_CONTEXT(php_v8_context);

    v8::MaybeLocal<v8::Value> maybe_local_res = local_function->Call(context, local_recv, argc, argv);

    if (argv) {
        efree(argv);
    }

    PHP_V8_MAYBE_CATCH(php_v8_context, try_catch);
    PHP_V8_THROW_VALUE_EXCEPTION_WHEN_EMPTY(maybe_local_res, "Failed to call");

    v8::Local<v8::Value> local_res = maybe_local_res.ToLocalChecked();

    php_v8_get_or_create_value(return_value, local_res, php_v8_value->php_v8_isolate);
}

static PHP_METHOD(V8Function, SetName) {
    zval *php_v8_string_zv;

    if (zend_parse_parameters(ZEND_NUM_ARGS(), "o", &php_v8_string_zv) == FAILURE) {
        return;
    }

    PHP_V8_VALUE_FETCH_WITH_CHECK(getThis(), php_v8_value);
    PHP_V8_VALUE_FETCH_WITH_CHECK(php_v8_string_zv, php_v8_string);

    PHP_V8_DATA_ISOLATES_CHECK(php_v8_value, php_v8_string);

    PHP_V8_ENTER_STORED_ISOLATE(php_v8_value);
    PHP_V8_ENTER_STORED_CONTEXT(php_v8_value);

    v8::Local<v8::Function> local_function = php_v8_value_get_local_as<v8::Function>(php_v8_value);
    v8::Local<v8::String> local_name = php_v8_value_get_local_as<v8::String>(php_v8_string);

    local_function->SetName(local_name);
}

static PHP_METHOD(V8Function, GetName) {
    if (zend_parse_parameters_none() == FAILURE) {
        return;
    }

    PHP_V8_VALUE_FETCH_WITH_CHECK(getThis(), php_v8_value);
    PHP_V8_ENTER_STORED_ISOLATE(php_v8_value);
    PHP_V8_ENTER_STORED_CONTEXT(php_v8_value);

    v8::Local<v8::Function> local_function = php_v8_value_get_local_as<v8::Function>(php_v8_value);
    v8::Local<v8::Value> local_name = local_function->GetName();

    php_v8_get_or_create_value(return_value, local_name, php_v8_value->php_v8_isolate);
}

static PHP_METHOD(V8Function, GetInferredName) {
    if (zend_parse_parameters_none() == FAILURE) {
        return;
    }

    PHP_V8_VALUE_FETCH_WITH_CHECK(getThis(), php_v8_value);
    PHP_V8_ENTER_STORED_ISOLATE(php_v8_value);
    PHP_V8_ENTER_STORED_CONTEXT(php_v8_value);

    v8::Local<v8::Function> local_function = php_v8_value_get_local_as<v8::Function>(php_v8_value);
    v8::Local<v8::Value> local_inferred_name = local_function->GetInferredName();

    php_v8_get_or_create_value(return_value, local_inferred_name, php_v8_value->php_v8_isolate);

}

static PHP_METHOD(V8Function, GetDisplayName) {
    if (zend_parse_parameters_none() == FAILURE) {
        return;
    }

    PHP_V8_VALUE_FETCH_WITH_CHECK(getThis(), php_v8_value);
    PHP_V8_ENTER_STORED_ISOLATE(php_v8_value);
    PHP_V8_ENTER_STORED_CONTEXT(php_v8_value);

    v8::Local<v8::Function> local_function = php_v8_value_get_local_as<v8::Function>(php_v8_value);
    v8::Local<v8::Value> local_display_name = local_function->GetDisplayName();

    php_v8_get_or_create_value(return_value, local_display_name, php_v8_value->php_v8_isolate);
}

static PHP_METHOD(V8Function, GetScriptLineNumber) {

    if (zend_parse_parameters_none() == FAILURE) {
        return;
    }

    PHP_V8_VALUE_FETCH_WITH_CHECK(getThis(), php_v8_value);
    PHP_V8_ENTER_STORED_ISOLATE(php_v8_value);
    PHP_V8_ENTER_STORED_CONTEXT(php_v8_value);

    v8::Local<v8::Function> local_function = php_v8_value_get_local_as<v8::Function>(php_v8_value);

    int line_number = local_function->GetScriptLineNumber();

    if (line_number == v8::Function::kLineOffsetNotFound) {
        RETURN_NULL();
    }

    RETURN_LONG((zend_long) line_number);
}

static PHP_METHOD(V8Function, GetScriptColumnNumber) {

    if (zend_parse_parameters_none() == FAILURE) {
        return;
    }

    PHP_V8_VALUE_FETCH_WITH_CHECK(getThis(), php_v8_value);
    PHP_V8_ENTER_STORED_ISOLATE(php_v8_value);
    PHP_V8_ENTER_STORED_CONTEXT(php_v8_value);

    v8::Local<v8::Function> local_function = php_v8_value_get_local_as<v8::Function>(php_v8_value);

    int column_number = local_function->GetScriptColumnNumber();

    if (column_number == v8::Function::kLineOffsetNotFound) {
        RETURN_NULL();
    }

    RETURN_LONG((zend_long) column_number);
}

static PHP_METHOD(V8Function, GetBoundFunction) {
    if (zend_parse_parameters_none() == FAILURE) {
        return;
    }

    PHP_V8_VALUE_FETCH_WITH_CHECK(getThis(), php_v8_value);
    PHP_V8_ENTER_STORED_ISOLATE(php_v8_value);
    PHP_V8_ENTER_STORED_CONTEXT(php_v8_value);

    v8::Local<v8::Function> local_function = php_v8_value_get_local_as<v8::Function>(php_v8_value);

    v8::Local<v8::Value> local_value = local_function->GetBoundFunction();

    php_v8_get_or_create_value(return_value, local_value, php_v8_value->php_v8_isolate);
}

static PHP_METHOD(V8Function, GetScriptOrigin) {
    if (zend_parse_parameters_none() == FAILURE) {
        return;
    }

    PHP_V8_VALUE_FETCH_WITH_CHECK(getThis(), php_v8_value);
    PHP_V8_ENTER_STORED_ISOLATE(php_v8_value);
    PHP_V8_ENTER_STORED_CONTEXT(php_v8_value);

    v8::Local<v8::Function> local_function = php_v8_value_get_local_as<v8::Function>(php_v8_value);

    v8::ScriptOrigin script_origin = local_function->GetScriptOrigin();

    php_v8_create_script_origin(return_value, context, script_origin);
}


ZEND_BEGIN_ARG_INFO_EX(arginfo_v8_function___construct, ZEND_SEND_BY_VAL, ZEND_RETURN_VALUE, 2)
                ZEND_ARG_OBJ_INFO(0, context, V8\\Context, 0)
                ZEND_ARG_CALLABLE_INFO(0, callback, 0)
                ZEND_ARG_TYPE_INFO(0, length, IS_LONG, 0)
ZEND_END_ARG_INFO()

PHP_V8_ZEND_BEGIN_ARG_WITH_RETURN_OBJ_INFO_EX(arginfo_v8_function_NewInstance, ZEND_RETURN_VALUE, 1, V8\\ObjectValue, 0)
                ZEND_ARG_OBJ_INFO(0, context, V8\\Context, 0)
                ZEND_ARG_ARRAY_INFO(0, arguments, 0)
ZEND_END_ARG_INFO()

PHP_V8_ZEND_BEGIN_ARG_WITH_RETURN_OBJ_INFO_EX(arginfo_v8_function_Call, ZEND_RETURN_VALUE, 2, V8\\Value, 0)
                ZEND_ARG_OBJ_INFO(0, context, V8\\Context, 0)
                ZEND_ARG_OBJ_INFO(0, recv, V8\\Value, 0)
                ZEND_ARG_ARRAY_INFO(0, arguments, 0)
ZEND_END_ARG_INFO()

// void method
ZEND_BEGIN_ARG_INFO_EX(arginfo_v8_function_SetName, ZEND_SEND_BY_VAL, ZEND_RETURN_VALUE, 1)
                ZEND_ARG_OBJ_INFO(0, name, V8\\StringValue, 0)
ZEND_END_ARG_INFO()

PHP_V8_ZEND_BEGIN_ARG_WITH_RETURN_OBJ_INFO_EX(arginfo_v8_function_GetName, ZEND_RETURN_VALUE, 0, V8\\Value, 0)
ZEND_END_ARG_INFO()

PHP_V8_ZEND_BEGIN_ARG_WITH_RETURN_OBJ_INFO_EX(arginfo_v8_function_GetInferredName, ZEND_RETURN_VALUE, 0, V8\\Value, 0)
ZEND_END_ARG_INFO()

PHP_V8_ZEND_BEGIN_ARG_WITH_RETURN_OBJ_INFO_EX(arginfo_v8_function_GetDisplayName, ZEND_RETURN_VALUE, 0, V8\\Value, 0)
ZEND_END_ARG_INFO()

// long or null
ZEND_BEGIN_ARG_INFO_EX(arginfo_v8_function_GetScriptLineNumber, ZEND_SEND_BY_VAL, ZEND_RETURN_VALUE, 0)
ZEND_END_ARG_INFO()

// long or null
ZEND_BEGIN_ARG_INFO_EX(arginfo_v8_function_GetScriptColumnNumber, ZEND_SEND_BY_VAL, ZEND_RETURN_VALUE, 0)
ZEND_END_ARG_INFO()

PHP_V8_ZEND_BEGIN_ARG_WITH_RETURN_OBJ_INFO_EX(arginfo_v8_function_GetBoundFunction, ZEND_RETURN_VALUE, 0, V8\\Value, 0)
ZEND_END_ARG_INFO()

PHP_V8_ZEND_BEGIN_ARG_WITH_RETURN_OBJ_INFO_EX(arginfo_v8_function_GetScriptOrigin, ZEND_RETURN_VALUE, 0, V8\\ScriptOrigin, 0)
ZEND_END_ARG_INFO()

static const zend_function_entry php_v8_object_methods[] = {
        PHP_ME(V8Function, __construct, arginfo_v8_function___construct, ZEND_ACC_PUBLIC | ZEND_ACC_CTOR)
        PHP_ME(V8Function, NewInstance, arginfo_v8_function_NewInstance, ZEND_ACC_PUBLIC)
        PHP_ME(V8Function, Call, arginfo_v8_function_Call, ZEND_ACC_PUBLIC)
        PHP_ME(V8Function, SetName, arginfo_v8_function_SetName, ZEND_ACC_PUBLIC)
        PHP_ME(V8Function, GetName, arginfo_v8_function_GetName, ZEND_ACC_PUBLIC)
        PHP_ME(V8Function, GetInferredName, arginfo_v8_function_GetInferredName, ZEND_ACC_PUBLIC)
        PHP_ME(V8Function, GetDisplayName, arginfo_v8_function_GetDisplayName, ZEND_ACC_PUBLIC)

        PHP_ME(V8Function, GetScriptLineNumber, arginfo_v8_function_GetScriptLineNumber, ZEND_ACC_PUBLIC)
        PHP_ME(V8Function, GetScriptColumnNumber, arginfo_v8_function_GetScriptColumnNumber, ZEND_ACC_PUBLIC)

        PHP_ME(V8Function, GetBoundFunction, arginfo_v8_function_GetBoundFunction, ZEND_ACC_PUBLIC)
        PHP_ME(V8Function, GetScriptOrigin, arginfo_v8_function_GetScriptOrigin, ZEND_ACC_PUBLIC)

        PHP_FE_END
};


PHP_MINIT_FUNCTION(php_v8_function) {
    zend_class_entry ce;
    INIT_NS_CLASS_ENTRY(ce, PHP_V8_NS, "FunctionObject", php_v8_object_methods);
    this_ce = zend_register_internal_class_ex(&ce, php_v8_object_class_entry);

    return SUCCESS;
}
