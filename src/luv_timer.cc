#include <assert.h>
#include "candor.h"
#include "uv.h"
#include "luv.h"
#include "luv_handle.h"
#include "luv_timer.h"

#include <stdio.h>
#include <string.h>

using namespace candor;
using namespace candorIO;

typedef candor::internal::List<char*, candor::internal::EmptyClass> List;

static Object* HandlePrototype;
static Object* TimerPrototype;
static Function* CloneScript;

static Object* ObjectClone(Object* source) {
  Value* argv[1];
  argv[0] = source;
  Value* result = CloneScript->Call(1, argv);
  if (!result->Is<Object>()) {
    printf("result is not an object!\n");
  }
  return result->As<Object>();
}

static Value* luv_create_timer(uint32_t argc, Arguments& argv) {
  Object* obj = ObjectClone(TimerPrototype);
  printf("obj %p\n", obj);
  CData* cdata = CData::New(sizeof(uv_timer_t));
  obj->Set("cdata", cdata);
  printf("cdata %p\n", cdata);
  uv_timer_t* timer = (uv_timer_t*)cdata->GetContents();
  uv_timer_init(uv_default_loop(), timer);
  printf("timer %p\n", timer);
  timer->data = obj;
  return obj;
}

static void luv_on_timer(uv_timer_t* handle, int status) {
  ((Object*)handle->data)->Get("onTimer")->As<Function>()->Call(0, NULL);
}

static Value* luv_timer_start(uint32_t argc, Arguments& argv) {
  printf("*argc %d\n", argc);
  assert(argc == 4 && argv[0]->Is<Object>() && argv[1]->Is<Number>() && argv[2]->Is<Number>() && argv[3]->Is<Function>());
  Object* obj = argv[0]->As<Object>();
  printf("*obj %p\n", obj);
  CData* cdata = obj->Get("cdata")->As<CData>();
  printf("*cdata %p\n", cdata);
  uv_timer_t* timer = (uv_timer_t*)cdata->GetContents();
  int64_t timeout = argv[1]->As<Number>()->IntegralValue();
  int64_t repeat = argv[2]->As<Number>()->IntegralValue();
  obj->Set("onTimer", argv[3]);
  uv_timer_start(timer, luv_on_timer, timeout, repeat);
  return Nil::New();
}

static Value* luv_timer_get_repeat(uint32_t argc, Arguments& argv) {
  assert(argc == 1 && argv[0]->Is<Object>());
  Object* obj = argv[0]->As<Object>();
  uv_timer_t* timer = (uv_timer_t*)obj->Get("cdata")->As<CData>()->GetContents();
  int64_t repeat = uv_timer_get_repeat(timer);
  return Number::NewIntegral(repeat);
}

static Value* luv_timer_set_repeat(uint32_t argc, Arguments& argv) {
  assert(argc == 2 && argv[0]->Is<CData>() && argv[1]->Is<Number>());
  Object* obj = argv[0]->As<Object>();
  uv_timer_t* timer = (uv_timer_t*)obj->Get("cdata")->As<CData>()->GetContents();
  int64_t repeat = argv[1]->As<Number>()->IntegralValue();
  uv_timer_set_repeat(timer, repeat);
  return Nil::New();
}

static Value* luv_timer_stop(uint32_t argc, Arguments& argv) {
  assert(argc == 1 && argv[0]->Is<Object>());
  Object* obj = argv[0]->As<Object>();
  uv_timer_t* timer = (uv_timer_t*)obj->Get("cdata")->As<CData>()->GetContents();
  uv_timer_stop(timer);
  return Nil::New();
}

static Value* luv_timer_again(uint32_t argc, Arguments& argv) {
  assert(argc == 1 && argv[0]->Is<Object>());
  Object* obj = argv[0]->As<Object>();
  uv_timer_t* timer = (uv_timer_t*)obj->Get("cdata")->As<CData>()->GetContents();
  uv_timer_again(timer);
  return Nil::New();
}

static void luv_on_close(uv_handle_t* handle) {
  ((Object*)handle->data)->Get("onClose")->As<Function>()->Call(0, NULL);
}

static Value* luv_close(uint32_t argc, Arguments& argv) {
  assert(argc  && argv[0]->Is<Object>());
  Object* obj = argv[0]->As<Object>();
  uv_handle_t* handle = (uv_handle_t*)obj->Get("cdata")->As<CData>()->GetContents();
  if (argc > 1 && argv[1]->Is<Function>()) {
    obj->Set("onClose", argv[1]);
  }
  uv_close(handle, luv_on_close);
  return Nil::New();
}


// Create the Timer object that wraps the C functions
void luv_timer_init(Object* uv) {

  // Compile the cloneScript
  const char* cloneScript = "return (source) {\ntarget = {}\nkeys = keysof source\nlength = sizeof keys\ni = 0\nwhile (i < length) {\nkey = keys[i]\nvalue = source[key]\ntarget[key] = value\ni++\n}\nreturn target\n}";
  CloneScript = Function::New(cloneScript, strlen(cloneScript));
  new Handle<Function>(CloneScript);
  CloneScript = CloneScript->Call(0, NULL)->As<Function>();
  new Handle<Function>(CloneScript);
  if (!CloneScript->Is<Function>()) {
    printf("CloneScript is not a function!\n");
  }

  // Create the Handle prototype
  HandlePrototype = Object::New();
  new Handle<Object>(HandlePrototype);
  HandlePrototype->Set("close", Function::New(luv_close));

  // Create the Timer prototype
  TimerPrototype = ObjectClone(HandlePrototype);
  new Handle<Object>(TimerPrototype);
  TimerPrototype->Set("start", Function::New(luv_timer_start));
  TimerPrototype->Set("stop", Function::New(luv_timer_stop));
  TimerPrototype->Set("again", Function::New(luv_timer_again));
  TimerPrototype->Set("getRepeat", Function::New(luv_timer_get_repeat));
  TimerPrototype->Set("setRepeat", Function::New(luv_timer_set_repeat));

  uv->Set("createTimer", Function::New(luv_create_timer));
}
