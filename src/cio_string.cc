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


void cio_string_init(Object* global) {
  Object* string = Object::New();
  global->Set("String", string);
  string->Set("readUInt8", Function::New(readUInt8));
  string->Set("readInt8", Function::New(readInt8));
  string->Set("readUInt16", Function::New(readUInt16));
  string->Set("readInt16", Function::New(readInt16));
  string->Set("readUInt32", Function::New(readUInt32));
  string->Set("readInt32", Function::New(readInt32));
  string->Set("readUInt64", Function::New(readUInt64));
  string->Set("readInt64", Function::New(readInt64));
}
