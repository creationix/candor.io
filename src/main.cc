#include "candor.h"
#include "uv.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

using namespace candor;

// internal flags
enum isTYPE {
  isHANDLE  = 0x01,
  isSTREAM  = 0x02,
  isTCP     = 0x04,
  isUDP     = 0x08,
  isPIPE    = 0x10,
  isTTY     = 0x20,
  isTIMER   = 0x40,
  isPROCESS = 0x80
};

enum kTYPE {
  kHANDLE   = isHANDLE,
  kSTREAM   = isHANDLE | isSTREAM,
  kTCP      = isHANDLE | isSTREAM | isTCP,
  kUDP      = isHANDLE | isUDP,
  kPIPE     = isHANDLE | isSTREAM | isPIPE,
  kTTY      = isHANDLE | isSTREAM | isTTY,
  kTIMER    = isHANDLE | isTIMER,
  kPROCESS  = isHANDLE | isPROCESS
};


static Value* NewTimer(uint32_t argc, Arguments& argv) {

  // Create a candor object with cdata and type
  // callbacks will go on this object later
  Handle<Object> self(Object::New());
  CData* cdata = CData::New(sizeof(uv_timer_t));
  self->Set(String::New("cdata", 5), cdata);
  self->Set(String::New("type", 4), Number::NewIntegral(kTIMER));

  // Initialize the libuv struct and point back to the whole object.
  uv_timer_t* handle = (uv_timer_t*)cdata->GetContents();
  uv_timer_init(uv_default_loop(), handle);
  handle->data = *self;

  return *self;
}

static void OnTimer(uv_timer_t* handle, int status) {
  Object* self = (Object*)handle->data;
  Value* callback = self->Get(String::New("onTimer", 7));
  if (callback->Is<Function>()) {
    callback->As<Function>()->Call(NULL, 0, NULL);
  }
}

static Value* TimerStart(uint32_t argc, Arguments& argv) {
  assert(argc == 3);
  Object* self = argv[0]->As<Object>();
  assert(self->Get(String::New("type", 4))->As<Number>()->IntegralValue() & isTIMER);
  uv_timer_t* handle = (uv_timer_t*)self->Get(String::New("cdata", 5))->As<CData>()->GetContents();

  int64_t timeout = argv[1]->As<Number>()->IntegralValue();
  int64_t repeat = argv[2]->As<Number>()->IntegralValue();

  uv_timer_start(handle, OnTimer, timeout, repeat);

  return Nil::New();
}

static void OnClose(uv_handle_t* handle) {
  Object* self = (Object*)handle->data;
  Value* callback = self->Get(String::New("onClose", 7));
  if (callback->Is<Function>()) {
    callback->As<Function>()->Call(NULL, 0, NULL);
  }
}

static Value* Close(uint32_t argc, Arguments& argv) {
  assert(argc == 1);
  Object* self = argv[0]->As<Object>();
  assert(self->Get(String::New("type", 4))->As<Number>()->IntegralValue() & isHANDLE);
  uv_handle_t* handle = (uv_handle_t*)self->Get(String::New("cdata", 5))->As<CData>()->GetContents();

  uv_close(handle, OnClose);

  return Nil::New();
}


static Value* Print(uint32_t argc, Arguments& argv) {
  // Print all arguments as strings with spaces and a newline.
  for (uint32_t i = 0; i < argc; i++) {
    Value* value = argv[i];
    const char* part;
    if (value->Is<Function>()) {
      part = "[Function]";
    } else if (value->Is<Object>()) {
      part = "[Object]";
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

  // Create a global args array.
  Object* uv = Object::New();
  global->Set(String::New("uv", 2), uv);

  uv->Set(String::New("newTimer", 8), Function::New(NewTimer));
  uv->Set(String::New("timerStart", 10), Function::New(TimerStart));
  uv->Set(String::New("close", 5), Function::New(Close));

  // Compile and run the script at argv[1]
  Handle<Function> script(compileScript(argv[1]));
  Value* result = script->Call(*global, 0, NULL);

  if (!result->Is<Nil>()) {
    printf("result: ");
    prettyPrint(result);
    printf("\n");
  }

  uv_run(uv_default_loop());

  return 0;
}