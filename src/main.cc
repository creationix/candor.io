#include "candor.h"
#include "uv.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

using namespace candor;


static Value* Print(uint32_t argc, Arguments& argv) {
  HandleScope scope;

  assert(argc == 1);

  Handle<String> str(argv[0]->ToString());
  printf("%s\n", str->Value());

  return Nil::New();
}

// Load a script from disk and compile it into a function
// This code *will* be replaced, it's just a hack to make testing easier.
static Function* compileScript(const char* filename) {
  FILE* fd;
  unsigned long length;
  char* data;
  size_t result;

  fd = fopen(filename , "rb" );
  if (fd == NULL) { fputs("File error", stderr); exit (1); }

  fseek (fd, 0, SEEK_END);
  length = ftell(fd);
  rewind(fd);

  data = (char*)malloc(sizeof(char) * (length + 1));
  if (data == NULL) { fputs ("Memory error", stderr); exit (2); }

  result = fread(data, 1, length, fd);
  if (result != length) { fputs ("Reading error", stderr); exit (3); }

  fclose(fd);

  Function* script = Function::New(data, length);
  free(data);

  return script;
}

// Simple pretty printer for validating function output
static void prettyPrint(Value* value) {
  if (value->Is<Nil>()) {
    printf("nil");
    return;
  }
  if (value->Is<Number>()) {
    Number* num = (Number*)value;
    if (num->IsIntegral()) {
      printf("%ld", num->IntegralValue());
    } else {
      printf("%f", num->Value());
    }
    return;
  }
  if (value->Is<Boolean>()) {
    Boolean* b = (Boolean*)value;
    if (b->IsTrue()) printf("true");
    if (b->IsFalse()) printf("false");
    return;
  }
  if (value->Is<String>()) {
    String* s = (String*)value;
    const char* value = s->Value();
    uint32_t length = s->Length();
    unsigned i;
    printf("\"");
    for (i = 0; i < length; i++) {
      switch (value[i]) {
        case '\0': printf("\\0"); break;
        case '\n': printf("\\n"); break;
        case '\r': printf("\\r"); break;
        case '\t': printf("\\t"); break;
        case '\b': printf("\\b"); break;
        case '\"': printf("\\\""); break;
        case '\\': printf("\\\\"); break;
        default:
          if (value[i] > 0x20 && value[i] < 0x80) {
            printf("%c", value[i]);
          } else {
            printf("\\%x", value[i]);
          }
      }
    }
    printf("\"");
    return;
  }
  if (value->Is<Function>()) {
    printf("[Function]");
    return;
  }
  if (value->Is<Object>()) {
    // Object* o = (Object*)value;
    // Value* argv[];
    // uint32_t argc;
    // o->Keys(&argc, &argv);

    printf("TODO: iterate over keys once there is an api");
    return;
  }
  printf("[unknown]");
}


int main(int argc, char** argv) {

  if (argc < 2) {
    printf("Usage:\n\t%s scriptname.can\n", argv[0]);
    exit(1);
  }

  // Create a new Isolate
  Isolate I;

  // Create a global context
  Handle<Object> global(Object::New());
  global->Set(String::New("print", 5), Function::New(Print));

  // Create a global args array.
  Object* args = Object::New();
  global->Set(String::New("args", 4), args);
  int i;
  for (i = 0; i < argc; i++) {
    const char* arg = argv[i];
    char buf[16];
    snprintf(buf, 16, "%d", i);
    args->Set(String::New(buf, strlen(buf)), String::New(arg, strlen(arg)));
  }

  // Compile and run the script at argv[1]
  Handle<Function> script(compileScript(argv[1]));
  Value* result = script->Call(*global, 0, NULL);

  if (!result->Is<Nil>()) {
    printf("result: ");
    prettyPrint(result);
    printf("\n");
  }

  // Start the libuv event loop
  uv_run(uv_default_loop());

  return 0;
}