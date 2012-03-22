#include "cio.h"

#include "candor.h"

#include "cio_string.h"   // cio_string_module
#include "lhttp_parser.h" // http_parser_module
#include "luv_base.h"     // uv_base_module
#include "luv_tcp.h"      // uv_tcp_module
#include "luv_timer.h"    // uv_timer_module

#include <assert.h> // assert
#include <string.h> // strcmp
#include <stdlib.h> // edit
#include <stdio.h>  // fprintf

using namespace candor;

// Print a value to a fd
static void printValue(FILE* fd, Value* value, bool shallow) {
  switch (value->Type()) {

    case Value::kString: {
      String* string = value->ToString();
      fprintf(fd, "\033[1;32m\"\033[0;32m%.*s\033[1;32m\"\033[0m", string->Length(), string->Value());
      return;
    }

    case Value::kNumber: {
      String* string = value->ToString();
      fprintf(fd, "\033[0;34m%.*s\033[0m", string->Length(), string->Value());
      return;
    }

    case Value::kBoolean: {
      String* string = value->ToString();
      fprintf(fd, "\033[0;33m%.*s\033[0m", string->Length(), string->Value());
      return;
    }

    case Value::kFunction: {
      fprintf(fd, "\033[0;36mfunction: %p\033[0m", value);
      break;
    }

    case Value::kObject: {
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

    case Value::kArray: {
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

    case Value::kCData: {
      fprintf(fd, "\033[0;31mcdata: %p\033[0m", value);
      return;
    }

    case Value::kNil: {
      fprintf(fd, "\033[1;30mnil\033[0m");
      return;
    }

    case Value::kNone: {
      fprintf(fd, "\033[1,31mnone\033[0m");
      return;
    }
  }
}

static Value* Print(uint32_t argc, Value* argv[]) {
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

static Value* PrettyPrint(uint32_t argc, Value* argv[]) {
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

static Value* Exit(uint32_t argc, Value* argv[]) {
  int status = 0;
  if (argc) {
    status = argv[0]->ToNumber()->IntegralValue();
  }
  exit(status);
  return Nil::New();
}

static Value* LoadBuiltin(uint32_t argc, Value* argv[]) {
  assert(argc == 1);
  const char* name = argv[0]->As<String>()->Value();
  if (0 == strcmp(name, "string")) return cio_string_module();
  if (0 == strcmp(name, "uv")) return uv_base_module();
  if (0 == strcmp(name, "timer")) return uv_timer_module();
  if (0 == strcmp(name, "tcp")) return uv_tcp_module();
  if (0 == strcmp(name, "http_parser")) return http_parser_module();
  return Nil::New();
}

void cio_init(Object* global) {
  global->Set("print", Function::New(Print));
  global->Set("prettyPrint", Function::New(PrettyPrint));
  global->Set("exit", Function::New(Exit));
  global->Set("require", Function::New(LoadBuiltin));
}
