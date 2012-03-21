#include "candor.h"
#include "http_parser.h"
#include "lhttp_parser.h"
#include <assert.h>
#include <string.h>
#include <stdio.h>

using namespace candor;

static http_parser_settings settings;

static int emit(http_parser* parser, const char* name) {
  Object* obj = (Object*)parser->data;
  Value* callback = obj->Get(name);
  if (callback->Is<Function>()) {
    Value* result = callback->As<Function>()->Call(0, NULL);
    return result->ToNumber()->IntegralValue();
  }
  return 0;
}

static int emit(http_parser* parser, const char* name, const char *at, size_t length) {
  Object* obj = (Object*)parser->data;
  Value* callback = obj->Get(name);
  if (callback->Is<Function>()) {
    Value* argv[1];
    argv[0] = String::New(at, length);
    Value* result = callback->As<Function>()->Call(1, argv);
    return result->ToNumber()->IntegralValue();
  }
  return 0;
}

static int lhttp_on_message_begin(http_parser* parser) {
  return emit(parser, "onMessageBegin");
}
static int lhttp_on_url(http_parser* parser, const char *at, size_t length) {
  return emit(parser, "onUrl", at, length);
}
static int lhttp_on_header_field(http_parser* parser, const char *at, size_t length) {
  return emit(parser, "onHeaderField", at, length);
}
static int lhttp_on_header_value(http_parser* parser, const char *at, size_t length) {
  return emit(parser, "onHeaderValue", at, length);
}
static int lhttp_on_headers_complete(http_parser* parser) {
  return emit(parser, "onHeadersComplete");
}
static int lhttp_on_body(http_parser* parser, const char *at, size_t length) {
  return emit(parser, "onBody", at, length);
}
static int lhttp_on_message_complete(http_parser* parser) {
  return emit(parser, "onMessageComplete");
}

static Value* lhttp_create(uint32_t argc, Arguments& argv) {
  assert(argc == 0);
  Object* obj = Object::New();
  CData* cdata = CData::New(sizeof(http_parser));
  http_parser* parser = (http_parser*)cdata->GetContents();
  parser->data = obj;
  obj->Set("cdata", cdata);
  return obj;
}

static Value* lhttp_init(uint32_t argc, Arguments& argv) {
  assert(argc >= 2 && argc <= 3);
  Object* obj = argv[0]->As<Object>();
  http_parser* parser = (http_parser*)obj->Get("cdata")->As<CData>()->GetContents();
  const char* type = argv[1]->As<String>()->Value();
  if (argc == 3) {
    Object* callbacks = argv[2]->As<Object>();
    Array* keys = callbacks->Keys();
    int64_t i = keys->Length();
    while (i--) {
      Value* key = keys->Get(i);
      obj->Set(key, callbacks->Get(key));
    }
  }
  if (0 == strcmp(type, "request")) {
    http_parser_init(parser, HTTP_REQUEST);
  } else if (0 == strcmp(type, "response")) {
    http_parser_init(parser, HTTP_RESPONSE);
  } else if (0 == strcmp(type, "both")) {
    http_parser_init(parser, HTTP_BOTH);
  } else {
    assert(false);
  }
  return Nil::New();
}

static Value* lhttp_execute(uint32_t argc, Arguments& argv) {
  assert(argc == 2);
  Object* obj = argv[0]->As<Object>();
  http_parser* parser = (http_parser*)obj->Get("cdata")->As<CData>()->GetContents();
  String* str = argv[1]->As<String>();
  const char* data = str->Value();
  size_t len = str->Length();
  size_t nparsed = http_parser_execute(parser, &settings, data, len);
  return Number::NewIntegral(nparsed);
}

static Value* lhttp_pause(uint32_t argc, Arguments& argv) {
  assert(argc == 2);
  Object* obj = argv[0]->As<Object>();
  http_parser* parser = (http_parser*)obj->Get("cdata")->As<CData>()->GetContents();
  int paused = argv[1]->As<Boolean>()->IsTrue();
  http_parser_pause(parser, paused);
  return Nil::New();
}

static Value* lhttp_parse_url(uint32_t argc, Arguments& argv) {
  assert(argc >= 1 && argv[0]->Is<String>());
  String* str = argv[0]->ToString();
  size_t buflen = str->Length();
  const char* buf = str->Value();
  int is_connect = 0;
  if (argc > 1 && argv[1]->Is<Boolean>()) {
    is_connect = argv[1]->ToBoolean()->IsTrue();
  }
  http_parser_url u;
  int status = http_parser_parse_url(buf, buflen, is_connect, &u);
  if (status) {
    return Number::NewIntegral(status);
  }

  Object* url = Object::New();
  if (u.field_set & (1 << UF_SCHEMA)) {
    url->Set("schema", String::New(buf + u.field_data[UF_SCHEMA].off, u.field_data[UF_SCHEMA].len));
  }
  if (u.field_set & (1 << UF_HOST)) {
    url->Set("host", String::New(buf + u.field_data[UF_HOST].off, u.field_data[UF_HOST].len));
  }
  if (u.field_set & (1 << UF_PORT)) {
    url->Set("port_string", String::New(buf + u.field_data[UF_PORT].off, u.field_data[UF_PORT].len));
    url->Set("port", Number::NewIntegral(u.port));
  }
  if (u.field_set & (1 << UF_PATH)) {
    url->Set("path", String::New(buf + u.field_data[UF_PATH].off, u.field_data[UF_PATH].len));
  }
  if (u.field_set & (1 << UF_QUERY)) {
    url->Set("query", String::New(buf + u.field_data[UF_QUERY].off, u.field_data[UF_QUERY].len));
  }
  if (u.field_set & (1 << UF_FRAGMENT)) {
    url->Set("fragment", String::New(buf + u.field_data[UF_FRAGMENT].off, u.field_data[UF_FRAGMENT].len));
  }
  
  return url;
}

static Handle<Object> module;
Object* http_parser_module() {
  if (!module.IsEmpty()) return *module;
  module.Wrap(Object::New());
  // Initialize the settings
  settings.on_message_begin = lhttp_on_message_begin;
  settings.on_url = lhttp_on_url;
  settings.on_header_field = lhttp_on_header_field;
  settings.on_header_value = lhttp_on_header_value;
  settings.on_headers_complete = lhttp_on_headers_complete;
  settings.on_body = lhttp_on_body;
  settings.on_message_complete = lhttp_on_message_complete;

  module->Set("parseUrl", Function::New(lhttp_parse_url));
  module->Set("create", Function::New(lhttp_create));
  return *module;
}

static Object* prototype;
Object* http_parser_prototype() {
  if (prototype) return prototype;
  prototype = Object::New();
  new Handle<Object>(prototype);
  prototype->Set("init", Function::New(lhttp_init));
  prototype->Set("execute", Function::New(lhttp_execute));
  prototype->Set("pause", Function::New(lhttp_pause));
  return prototype;
}




