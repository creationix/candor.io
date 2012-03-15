#include "cio.h"
#include "candor.h"
#include "stdio.h"
#include "stdlib.h"
#include "string.h"

using namespace candor;

static Value* Print(uint32_t argc, Arguments& argv) {
  // Print all arguments as strings with spaces and a newline.
  for (uint32_t i = 0; i < argc; i++) {
    Value* value = argv[i];
    const char* part;
    uint32_t length;
    if (value->Is<Function>()) {
      part = "[Function]";
      length = 10;
    } else if (value->Is<Object>()) {
      part = "[Object]";
      length = 8;
    } else if (value->Is<Array>()) {
      part = "[Array]";
      length = 7;
    } else if (value->Is<CData>()) {
      part = "[CData]";
      length = 7;
    } else if (value->Is<Nil>()) {
      part = "nil";
      length = 3;
    } else {
      String* string = value->ToString();
      part = string->Value();
      length = string->Length();
    }

    if (i == argc - 1) {
      printf("%.*s\n", length, part);
    } else {
      printf("%.*s ", length, part);
    }
  }
  return Nil::New();
}

void cio_init(Object* global) {
  global->Set("print", Function::New(Print));
}
