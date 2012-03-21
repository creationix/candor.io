#include <assert.h>
#include <stdio.h>
#include "candor.h"
#include "cio_string.h"

static Value* readUInt8(uint32_t argc, Arguments& argv) {
  assert(argc == 2 && argv[0]->Is<String>() && argv[1]->Is<Number>());
  String* str = argv[0]->ToString();
  unsigned int offset = argv[1]->ToNumber()->IntegralValue();
  assert(offset >=0 && offset < str->Length());
  return Number::NewIntegral(*(uint8_t*)((uint8_t*)str->Value() + offset));
}

static Value* readInt8(uint32_t argc, Arguments& argv) {
  assert(argc == 2 && argv[0]->Is<String>() && argv[1]->Is<Number>());
  String* str = argv[0]->ToString();
  unsigned int offset = argv[1]->ToNumber()->IntegralValue();
  assert(offset >=0 && offset < str->Length());
  return Number::NewIntegral(*(int8_t*)((uint8_t*)str->Value() + offset));
}

static Value* readUInt16(uint32_t argc, Arguments& argv) {
  assert(argc == 2 && argv[0]->Is<String>() && argv[1]->Is<Number>());
  String* str = argv[0]->ToString();
  unsigned int offset = argv[1]->ToNumber()->IntegralValue();
  assert(offset >=0 && offset + 1 < str->Length());
  return Number::NewIntegral(*(uint16_t*)((uint8_t*)str->Value() + offset));
}

static Value* readInt16(uint32_t argc, Arguments& argv) {
  assert(argc == 2 && argv[0]->Is<String>() && argv[1]->Is<Number>());
  String* str = argv[0]->ToString();
  unsigned int offset = argv[1]->ToNumber()->IntegralValue();
  assert(offset >=0 && offset + 1 < str->Length());
  return Number::NewIntegral(*(int16_t*)((uint8_t*)str->Value() + offset));
}

static Value* readUInt32(uint32_t argc, Arguments& argv) {
  assert(argc == 2 && argv[0]->Is<String>() && argv[1]->Is<Number>());
  String* str = argv[0]->ToString();
  unsigned int offset = argv[1]->ToNumber()->IntegralValue();
  assert(offset >=0 && offset + 3 < str->Length());
  return Number::NewIntegral(*(uint32_t*)((uint8_t*)str->Value() + offset));
}

static Value* readInt32(uint32_t argc, Arguments& argv) {
  assert(argc == 2 && argv[0]->Is<String>() && argv[1]->Is<Number>());
  String* str = argv[0]->ToString();
  unsigned int offset = argv[1]->ToNumber()->IntegralValue();
  assert(offset >=0 && offset + 3 < str->Length());
  return Number::NewIntegral(*(int32_t*)((uint8_t*)str->Value() + offset));
}

static Value* readUInt64(uint32_t argc, Arguments& argv) {
  assert(argc == 2 && argv[0]->Is<String>() && argv[1]->Is<Number>());
  String* str = argv[0]->ToString();
  unsigned int offset = argv[1]->ToNumber()->IntegralValue();
  assert(offset >=0 && offset + 7 < str->Length());
  return Number::NewIntegral(*(uint64_t*)((uint8_t*)str->Value() + offset));
}

static Value* readInt64(uint32_t argc, Arguments& argv) {
  assert(argc == 2 && argv[0]->Is<String>() && argv[1]->Is<Number>());
  String* str = argv[0]->ToString();
  unsigned int offset = argv[1]->ToNumber()->IntegralValue();
  assert(offset >=0 && offset + 7 < str->Length());
  return Number::NewIntegral(*(int64_t*)((uint8_t*)str->Value() + offset));
}

static Handle<Object> module;
Object* cio_string_module() {
  if (!module.IsEmpty()) return *module;
  module.Wrap(Object::New());
  // These match the endianess of the host.
  // TODO: add functions using explicit endianess
  module->Set("readUInt8", Function::New(readUInt8));
  module->Set("readInt8", Function::New(readInt8));
  module->Set("readUInt16", Function::New(readUInt16));
  module->Set("readInt16", Function::New(readInt16));
  module->Set("readUInt32", Function::New(readUInt32));
  module->Set("readInt32", Function::New(readInt32));
  module->Set("readUInt64", Function::New(readUInt64));
  module->Set("readInt64", Function::New(readInt64));
  return *module;
}
