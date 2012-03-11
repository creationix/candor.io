#include "cio.h"
#include "candor.h"
#include "stdio.h"
#include "stdlib.h"

using namespace candor;

static Value* Print(uint32_t argc, Arguments& argv) {
  // Print all arguments as strings with spaces and a newline.
  for (uint32_t i = 0; i < argc; i++) {
    Value* value = argv[i];
    const char* part;
    if (value->Is<Function>()) {
      part = "[Function]";
    } else if (value->Is<Object>()) {
      part = "[Object]";
    } else if (value->Is<Array>()) {
      part = "[Array]";
    } else if (value->Is<CData>()) {
      part = "[CData]";
    } else if (value->Is<Nil>()) {
      part = "nil";
    } else {
      part = value->ToString()->Value();
    }

    if (i == argc - 1) {
      printf("%s\n", part);
    } else {
      printf("%s ", part);
    }
  }
  return Nil::New();
}

void cio_init(Object* global) {
  global->Set(String::New("print", 5), Function::New(Print));
}
