#include "cio.h"
#include "candor.h"
#include "stdio.h"
#include "stdlib.h"
#include "string.h"

#include <assert.h>
#include <stdio.h>

using namespace candor;

static Object* builtins;
static Object* builtinCache;

// Print a value to a fd
static void printValue(FILE* fd, Value* value, bool shallow) {
  if (value->Is<String>()) {
    String* string = value->ToString();
    fprintf(fd, "\033[1;32m\"\033[0;32m%.*s\033[1;32m\"\033[0m", string->Length(), string->Value());
    return;
  }
  if (value->Is<Number>()) {
    String* string = value->ToString();
    fprintf(fd, "\033[0;34m%.*s\033[0m", string->Length(), string->Value());
    return;
  }
  if (value->Is<Boolean>()) {
    String* string = value->ToString();
    fprintf(fd, "\033[0;33m%.*s\033[0m", string->Length(), string->Value());
    return;
  }
  if (value->Is<Function>()) {
    fprintf(fd, "\033[0;36mfunction: %p\033[0m", value);
    return;
  }

  if (value->Is<Object>()) {
    if (shallow) {
      fprintf(fd, "\033[0;35mobject: %p\033[0m", value);
      return;
    }
    Object* obj = value->As<Object>();
    Array* keys = obj->Keys();
    int64_t length = keys->Length();
    int64_t i = 0;
    fprintf(fd, "\033[1;37m{\033[0m ");
    while (i < length) {
      Value* key = keys->Get(i);
      if (key->Is<String>()) {
        String* string = key->ToString();
        fprintf(fd, "%.*s", string->Length(), string->Value());
      } else {
        fprintf(fd, "[");
        printValue(fd, key, true);
        fprintf(fd, "]");
      }

      fprintf(fd, "\033[1;37m:\033[0m ");
      printValue(fd, obj->Get(key), true);
      if (i < length - 1) {
        fprintf(fd, "\033[1;37m,\033[0m ");
      }
      i++;
    }
    fprintf(fd, " \033[1;37m}\033[0m");
    return;
  }

  if (value->Is<Array>()) {
    if (shallow) {
      fprintf(fd, "\033[0;36marray: %p\033[0m", value);
      return;
    }
    Array* array = value->As<Array>();
    int64_t length = array->Length();
    int64_t i = 0;
    fprintf(fd, "[ ");
    while (i < length) {
      printValue(fd, array->Get(i), true);
      if (i < length - 1) {
        fprintf(fd, ", ");
      }
      i++;
    }
    fprintf(fd, " ]");
    return;
  }

  if (value->Is<CData>()) {
    fprintf(fd, "\033[0;31mcdata: %p\033[0m", value);
    return;
  }

  if (value->Is<Nil>()) {
    fprintf(fd, "\033[1;30mnil\033[0m");
    return;
  }

  assert(false);
}

static Value* Print(uint32_t argc, Arguments& argv) {
  // Print all arguments as strings with spaces and a newline.
  for (uint32_t i = 0; i < argc; i++) {
    String* string = argv[i]->ToString();
    fprintf(stdout, "%.*s", string->Length(), string->Value());
    if (i < argc - 1) {
      fprintf(stdout, ", ");
    }
  }
  fprintf(stdout, "\n");
  return Nil::New();
}

static Value* PrettyPrint(uint32_t argc, Arguments& argv) {
  // Print all arguments as strings with spaces and a newline.
  for (uint32_t i = 0; i < argc; i++) {
    printValue(stdout, argv[i], false);
    if (i < argc - 1) {
      fprintf(stdout, ", ");
    }
  }
  fprintf(stdout, "\n");
  return Nil::New();
}

static Value* Exit(uint32_t argc, Arguments& argv) {
  int status = 0;
  if (argc) {
    status = argv[0]->ToNumber()->IntegralValue();
  }
  exit(status);
  return Nil::New();
}

static Value* LoadBuiltin(uint32_t argc, Arguments& argv) {
  assert(argc == 1 && argv[0]->Is<String>());
  String* name = argv[0]->As<String>();
  Value* cache = builtinCache->Get(name);
  if (!cache->Is<Nil>()) {
    return cache;
  }
  Value* obj = builtins->Get(name);
  assert(obj->Is<Function>());
  Function* setup = obj->As<Function>();
  Value* module = setup->Call(0, NULL);
  if (!module->Is<Nil>()) {
    builtinCache->Set(name, module);
  }
  return module;
}

Object* cio_init(Object* global) {
  global->Set("print", Function::New(Print));
  global->Set("prettyPrint", Function::New(PrettyPrint));
  global->Set("exit", Function::New(Exit));
  global->Set("require", Function::New(LoadBuiltin));
  builtins = Object::New();
  builtinCache = Object::New();
  // Make the objects persistent
  new Handle<Object>(builtins);
  new Handle<Object>(builtinCache);

  return builtins;
}
