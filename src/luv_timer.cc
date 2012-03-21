#include "candor.h"
#include "uv.h"
#include "luv_handle.h"
#include "luv_timer.h"

#include <assert.h>

using namespace candor;

// TODO: remove when candor API has this.
static Object* CloneObject(Object* original) {
  Object* clone = Object::New();
  Array* keys = original->Keys();
  int64_t i = keys->Length();
  while (i--) {
    Value* key = keys->Get(i);
    clone->Set(key, original->Get(key));
  }
  return clone;
}

static Value* luv_create_timer(uint32_t argc, Arguments& argv) {
  assert(argc == 0);
  Object* obj = CloneObject(uv_timer_prototype());
  CData* cdata = CData::New(sizeof(uv_timer_t));
  uv_timer_t* handle = (uv_timer_t*)cdata->GetContents();
  handle->data = obj;
  obj->Set("cdata", cdata);
  uv_timer_init(uv_default_loop(), handle);
  return obj;
}

static void luv_on_timer(uv_timer_t* handle, int status) {
  Object* obj = (Object*)handle->data;
  Value* callback = obj->Get("onTimer");
  if (callback->Is<Function>()) {
    Value* argv[1];
    argv[0] = Number::NewIntegral(status);
    callback->As<Function>()->Call(1, argv);
  }
}

static Value* luv_timer_start(uint32_t argc, Arguments& argv) {
  assert(argc >= 3 && argc <= 4);
  Object* obj = argv[0]->As<Object>();
  uv_timer_t* handle = (uv_timer_t*)obj->Get("cdata")->As<CData>()->GetContents();
  int64_t timeout = argv[1]->As<Number>()->IntegralValue();
  int64_t repeat = argv[2]->As<Number>()->IntegralValue();
  if (argc == 4) {
    obj->Set("onTimer", argv[3]->As<Function>());
  }
  int status = uv_timer_start(handle, luv_on_timer, timeout, repeat);
  return Number::NewIntegral(status);
}

static Value* luv_timer_stop(uint32_t argc, Arguments& argv) {
  assert(argc == 1);
  Object* obj = argv[0]->As<Object>();
  uv_timer_t* handle = (uv_timer_t*)obj->Get("cdata")->As<CData>()->GetContents();
  int status = uv_timer_stop(handle);
  return Number::NewIntegral(status);
}

static Value* luv_timer_again(uint32_t argc, Arguments& argv) {
  assert(argc == 1);
  Object* obj = argv[0]->As<Object>();
  uv_timer_t* handle = (uv_timer_t*)obj->Get("cdata")->As<CData>()->GetContents();
  int status = uv_timer_again(handle);
  return Number::NewIntegral(status);
}

static Value* luv_timer_get_repeat(uint32_t argc, Arguments& argv) {
  assert(argc == 1);
  Object* obj = argv[0]->As<Object>();
  uv_timer_t* handle = (uv_timer_t*)obj->Get("cdata")->As<CData>()->GetContents();
  int64_t repeat = uv_timer_get_repeat(handle);
  return Number::NewIntegral(repeat);
}

static Value* luv_timer_set_repeat(uint32_t argc, Arguments& argv) {
  assert(argc == 2);
  Object* obj = argv[0]->As<Object>();
  uv_timer_t* handle = (uv_timer_t*)obj->Get("cdata")->As<CData>()->GetContents();
  int64_t repeat = argv[1]->As<Number>()->IntegralValue();
  uv_timer_set_repeat(handle, repeat);
  return Nil::New();
}

static Object* module;
Object* uv_timer_module() {
  if (module) return module;
  module = Object::New();
  new Handle<Object>(module);
  module->Set("create", Function::New(luv_create_timer));  
  return module;
}

static Object* prototype;
Object* uv_timer_prototype() {
  if (prototype) return prototype;
  prototype = CloneObject(uv_handle_prototype());
  new Handle<Object>(prototype);
  prototype->Set("start", Function::New(luv_timer_start));
  prototype->Set("stop", Function::New(luv_timer_stop));
  prototype->Set("again", Function::New(luv_timer_again));
  prototype->Set("getRepeat", Function::New(luv_timer_get_repeat));
  prototype->Set("setRepeat", Function::New(luv_timer_set_repeat));
  return prototype;
}

