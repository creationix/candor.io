#include "candor.h"
#include "http_parser.h"
#include "lhttp_parser.h"
#include <assert.h>

static Value* ParseUrl(uint32_t argc, Arguments& argv) {
  assert(argc >= 1 && argv[0]->Is<String>());
  String* str = argv[0]->ToString();
  size_t buflen = str->Length();
  const char* buf = str->Value();
  int is_connect = 0;
  if (argc > 1 && argv[1]->Is<Boolean>()) {
    is_connect = argv[1]->ToBoolean()->IsTrue() ? 1 : 0;
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

void lhttp_parser_init(Object* global) {

  Object* http_parser = Object::New();
  global->Set("HttpParser", http_parser);
  http_parser->Set("parseUrl", Function::New(ParseUrl));

}

