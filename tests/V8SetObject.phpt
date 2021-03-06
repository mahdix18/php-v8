--TEST--
V8\SetObject
--SKIPIF--
<?php if (!extension_loaded("v8")) print "skip"; ?>
--FILE--
<?php

/** @var \Phpv8Testsuite $helper */
$helper = require '.testsuite.php';

require '.v8-helpers.php';
$v8_helper = new PhpV8Helpers($helper);

$isolate = new \V8\Isolate();
$global_template = new V8\ObjectTemplate($isolate);

$context = new V8\Context($isolate, $global_template);

$value = new V8\SetObject($context);


$helper->header('Object representation');
$helper->dump($value);
$helper->space();

$helper->assert('SetObject extends Value', $value instanceof \V8\Value);
$helper->assert('SetObject does not extend PrimitiveValue', !($value instanceof \V8\PrimitiveValue));
$helper->assert('SetObject implements AdjustableExternalMemoryInterface', $value instanceof \V8\AdjustableExternalMemoryInterface);
$helper->assert('SetObject is instanceof Set', $value->InstanceOf($context, $context->GlobalObject()->Get($context, new \V8\StringValue($isolate, 'Set'))));
$helper->line();

$helper->header('Accessors');
$helper->method_matches($value, 'GetIsolate', $isolate);
$helper->method_matches($value, 'GetContext', $context);
$helper->space();

$helper->header('Getters');
$helper->assert('GetIdentityHash is integer', gettype($value->GetIdentityHash()), 'integer');
$helper->space();

$v8_helper->run_checks($value, 'Checkers');

$helper->header('Converters');
$helper->dump_object_methods($value, ['@@default' => [$context]], new RegexpFilter('/^To/'));
$helper->space();


$helper->header('New value creation from V8 runtime');
$filter = new ArrayListFilter(['IsObject', 'IsMap', 'IsWeakMap', 'IsSet', 'IsWeakSet'], false);
$new_map = $v8_helper->CompileRun($context, "new Set()");
$helper->assert('New set from V8 is instance of \V8\SetObject', $new_map instanceof \V8\SetObject);
$helper->dump_object_methods($new_map, [], $filter);
$helper->line();

$new_map = $v8_helper->CompileRun($context, "new WeakSet()");
$helper->assert('New weak set from V8 is NOT an instance of \V8\SetObject', $new_map instanceof \V8\SetObject, false);
$helper->dump_object_methods($new_map, [], $filter);
$helper->space();


$helper->header('Class-specific methods');

$key = new \V8\ObjectValue($context);
$key2 = new \V8\ObjectValue($context);
$nonexistent_key = new \V8\ObjectValue($context);

$helper->method_export($value, 'Size');
$helper->assert('Can add key', $value->Add($context, $key), $value);
$helper->assert('Can add another different key', $value->Add($context, $key2), $value);
$helper->method_export($value, 'Size');
$helper->assert('Cannot add another same key', $value->Add($context, $key2), $value);
$helper->method_export($value, 'Size');
$helper->line();


$helper->assert('Key exists', $value->Has($context, $key));
$helper->assert('Another key exists', $value->Has($context, $key2));

$helper->assert('Nonexistent key does not exists', $value->Has($context, $nonexistent_key), false);
$helper->line();

$helper->method_export($value, 'Size');
$helper->method_matches_instanceof($value, 'AsArray', \V8\ArrayObject::class);
$helper->line();

$arr = $value->AsArray();
$helper->assert('SetObject Array representation has valid length', $arr->Length() == 2);
$helper->assert('SetObject Array contains key', $arr->Get($context, new \V8\Uint32Value($isolate, 0)), $key);
$helper->assert('SetObject Array contains another key', $arr->Get($context, new \V8\Uint32Value($isolate, 1)), $key2);
$helper->line();

$helper->assert('Delete existent key', $value->Delete($context, $key));
$helper->assert('Deleted key does not exists', $value->Has($context, $key), false);
$helper->assert('Delete nonexistent key fails', $value->Delete($context, $nonexistent_key), false);
$helper->assert('Deleted nonexistent key does not exists', $value->Has($context, $nonexistent_key), false);
$helper->method_export($value, 'Size');
$helper->line();

$value->Add($context, new \V8\ObjectValue($context));
$value->Add($context, new \V8\NumberValue($isolate, 42));
$helper->method_export($value, 'Size');
$helper->method_export($value, 'Clear');
$helper->method_export($value, 'Size');


?>
--EXPECT--
Object representation:
----------------------
object(V8\SetObject)#6 (2) {
  ["isolate":"V8\Value":private]=>
  object(V8\Isolate)#3 (0) {
  }
  ["context":"V8\ObjectValue":private]=>
  object(V8\Context)#5 (1) {
    ["isolate":"V8\Context":private]=>
    object(V8\Isolate)#3 (0) {
    }
  }
}


SetObject extends Value: ok
SetObject does not extend PrimitiveValue: ok
SetObject implements AdjustableExternalMemoryInterface: ok
SetObject is instanceof Set: ok

Accessors:
----------
V8\SetObject::GetIsolate() matches expected value
V8\SetObject::GetContext() matches expected value


Getters:
--------
GetIdentityHash is integer: ok


Checkers:
---------
V8\SetObject(V8\Value)->TypeOf(): V8\StringValue->Value(): string(6) "object"

V8\SetObject(V8\ObjectValue)->IsCallable(): bool(false)
V8\SetObject(V8\ObjectValue)->IsConstructor(): bool(false)
V8\SetObject(V8\Value)->IsUndefined(): bool(false)
V8\SetObject(V8\Value)->IsNull(): bool(false)
V8\SetObject(V8\Value)->IsNullOrUndefined(): bool(false)
V8\SetObject(V8\Value)->IsTrue(): bool(false)
V8\SetObject(V8\Value)->IsFalse(): bool(false)
V8\SetObject(V8\Value)->IsName(): bool(false)
V8\SetObject(V8\Value)->IsString(): bool(false)
V8\SetObject(V8\Value)->IsSymbol(): bool(false)
V8\SetObject(V8\Value)->IsFunction(): bool(false)
V8\SetObject(V8\Value)->IsArray(): bool(false)
V8\SetObject(V8\Value)->IsObject(): bool(true)
V8\SetObject(V8\Value)->IsBoolean(): bool(false)
V8\SetObject(V8\Value)->IsNumber(): bool(false)
V8\SetObject(V8\Value)->IsInt32(): bool(false)
V8\SetObject(V8\Value)->IsUint32(): bool(false)
V8\SetObject(V8\Value)->IsDate(): bool(false)
V8\SetObject(V8\Value)->IsArgumentsObject(): bool(false)
V8\SetObject(V8\Value)->IsBooleanObject(): bool(false)
V8\SetObject(V8\Value)->IsNumberObject(): bool(false)
V8\SetObject(V8\Value)->IsStringObject(): bool(false)
V8\SetObject(V8\Value)->IsSymbolObject(): bool(false)
V8\SetObject(V8\Value)->IsNativeError(): bool(false)
V8\SetObject(V8\Value)->IsRegExp(): bool(false)
V8\SetObject(V8\Value)->IsAsyncFunction(): bool(false)
V8\SetObject(V8\Value)->IsGeneratorFunction(): bool(false)
V8\SetObject(V8\Value)->IsGeneratorObject(): bool(false)
V8\SetObject(V8\Value)->IsPromise(): bool(false)
V8\SetObject(V8\Value)->IsMap(): bool(false)
V8\SetObject(V8\Value)->IsSet(): bool(true)
V8\SetObject(V8\Value)->IsMapIterator(): bool(false)
V8\SetObject(V8\Value)->IsSetIterator(): bool(false)
V8\SetObject(V8\Value)->IsWeakMap(): bool(false)
V8\SetObject(V8\Value)->IsWeakSet(): bool(false)
V8\SetObject(V8\Value)->IsArrayBuffer(): bool(false)
V8\SetObject(V8\Value)->IsArrayBufferView(): bool(false)
V8\SetObject(V8\Value)->IsTypedArray(): bool(false)
V8\SetObject(V8\Value)->IsUint8Array(): bool(false)
V8\SetObject(V8\Value)->IsUint8ClampedArray(): bool(false)
V8\SetObject(V8\Value)->IsInt8Array(): bool(false)
V8\SetObject(V8\Value)->IsUint16Array(): bool(false)
V8\SetObject(V8\Value)->IsInt16Array(): bool(false)
V8\SetObject(V8\Value)->IsUint32Array(): bool(false)
V8\SetObject(V8\Value)->IsInt32Array(): bool(false)
V8\SetObject(V8\Value)->IsFloat32Array(): bool(false)
V8\SetObject(V8\Value)->IsFloat64Array(): bool(false)
V8\SetObject(V8\Value)->IsDataView(): bool(false)
V8\SetObject(V8\Value)->IsSharedArrayBuffer(): bool(false)
V8\SetObject(V8\Value)->IsProxy(): bool(false)


Converters:
-----------
V8\SetObject(V8\Value)->ToBoolean():
    object(V8\BooleanValue)#121 (1) {
      ["isolate":"V8\Value":private]=>
      object(V8\Isolate)#3 (0) {
      }
    }
V8\SetObject(V8\Value)->ToNumber():
    object(V8\NumberValue)#121 (1) {
      ["isolate":"V8\Value":private]=>
      object(V8\Isolate)#3 (0) {
      }
    }
V8\SetObject(V8\Value)->ToString():
    object(V8\StringValue)#121 (1) {
      ["isolate":"V8\Value":private]=>
      object(V8\Isolate)#3 (0) {
      }
    }
V8\SetObject(V8\Value)->ToDetailString():
    object(V8\StringValue)#121 (1) {
      ["isolate":"V8\Value":private]=>
      object(V8\Isolate)#3 (0) {
      }
    }
V8\SetObject(V8\Value)->ToObject():
    object(V8\SetObject)#6 (2) {
      ["isolate":"V8\Value":private]=>
      object(V8\Isolate)#3 (0) {
      }
      ["context":"V8\ObjectValue":private]=>
      object(V8\Context)#5 (1) {
        ["isolate":"V8\Context":private]=>
        object(V8\Isolate)#3 (0) {
        }
      }
    }
V8\SetObject(V8\Value)->ToInteger():
    object(V8\Int32Value)#121 (1) {
      ["isolate":"V8\Value":private]=>
      object(V8\Isolate)#3 (0) {
      }
    }
V8\SetObject(V8\Value)->ToUint32():
    object(V8\Int32Value)#121 (1) {
      ["isolate":"V8\Value":private]=>
      object(V8\Isolate)#3 (0) {
      }
    }
V8\SetObject(V8\Value)->ToInt32():
    object(V8\Int32Value)#121 (1) {
      ["isolate":"V8\Value":private]=>
      object(V8\Isolate)#3 (0) {
      }
    }
V8\SetObject(V8\Value)->ToArrayIndex(): V8\Exceptions\Exception: Failed to convert


New value creation from V8 runtime:
-----------------------------------
New set from V8 is instance of \V8\SetObject: ok
V8\SetObject(V8\Value)->IsObject(): bool(true)
V8\SetObject(V8\Value)->IsMap(): bool(false)
V8\SetObject(V8\Value)->IsSet(): bool(true)
V8\SetObject(V8\Value)->IsWeakMap(): bool(false)
V8\SetObject(V8\Value)->IsWeakSet(): bool(false)

New weak set from V8 is NOT an instance of \V8\SetObject: ok
V8\ObjectValue(V8\Value)->IsObject(): bool(true)
V8\ObjectValue(V8\Value)->IsMap(): bool(false)
V8\ObjectValue(V8\Value)->IsSet(): bool(false)
V8\ObjectValue(V8\Value)->IsWeakMap(): bool(false)
V8\ObjectValue(V8\Value)->IsWeakSet(): bool(true)


Class-specific methods:
-----------------------
V8\SetObject->Size(): float(0)
Can add key: ok
Can add another different key: ok
V8\SetObject->Size(): float(2)
Cannot add another same key: ok
V8\SetObject->Size(): float(2)

Key exists: ok
Another key exists: ok
Nonexistent key does not exists: ok

V8\SetObject->Size(): float(2)
V8\SetObject::AsArray() result is instance of V8\ArrayObject

SetObject Array representation has valid length: ok
SetObject Array contains key: ok
SetObject Array contains another key: ok

Delete existent key: ok
Deleted key does not exists: ok
Delete nonexistent key fails: ok
Deleted nonexistent key does not exists: ok
V8\SetObject->Size(): float(1)

V8\SetObject->Size(): float(3)
V8\SetObject->Clear(): NULL
V8\SetObject->Size(): float(0)
