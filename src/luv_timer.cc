#include <assert.h>
#include "candor.h"
#include "uv.h"
#include "luv.h"
#include "luv_handle.h"
#include "luv_handle.h"
#include "luv_timer.h"

#include <stdio.h>
#include <string.h>

using namespace candor;
using namespace candorIO;

void uvTimer::Initialize() {
  uv_timer_init(uv_default_loop(), &handle);
  handle.data = this;
}

void uvTimer::OnTimer(uv_timer_t* handle, int status) {
  uvTimer* self = reinterpret_cast<uvTimer*>(handle->data);
  self->onTimer->Call(0, NULL);
}

Value* uvTimer::Start(uint32_t argc, Arguments& argv) {
  assert(this == handle.data);
  assert(argc == 4 && argv[1]->Is<Number>() && argv[2]->Is<Number>() && argv[3]->Is<Function>());
  int64_t timeout = argv[1]->As<Number>()->IntegralValue();
  int64_t repeat = argv[2]->As<Number>()->IntegralValue();
  onTimer = argv[3]->As<Function>();
  uv_timer_start(&handle, OnTimer, timeout, repeat);
  return Nil::New();
}

Value* uvTimer::GetRepeat(uint32_t argc, Arguments& argv) {
  assert(argc == 1);
  int64_t repeat = uv_timer_get_repeat(&handle);
  return Number::NewIntegral(repeat);
}

Value* uvTimer::SetRepeat(uint32_t argc, Arguments& argv) {
  assert(argc == 2 && argv[1]->Is<Number>());
  int64_t repeat = argv[1]->As<Number>()->IntegralValue();
  uv_timer_set_repeat(&handle, repeat);
  return Nil::New();
}

Value* uvTimer::Stop(uint32_t argc, Arguments& argv) {
  assert(argc == 1);
  uv_timer_stop(&handle);
  return Nil::New();
}

Value* uvTimer::Again(uint32_t argc, Arguments& argv) {
  assert(argc == 1);
  uv_timer_again(&handle);
  return Nil::New();
}

void uvTimer::WeakCallback(CData* cdata) {
  printf("timer collecting %p\n", cdata);
  uvTimer* timer = reinterpret_cast<uvTimer*>(cdata->GetContents());
  delete timer;
}

uvTimer* uvTimer::Unwrap(Value* value) {
  return reinterpret_cast<uvTimer*>(value->As<CData>()->GetContents());
}


static Value* luv_create_timer(uint32_t argc, Arguments& argv) {
  assert(argc == 0);
  CData* cdata = CData::New(sizeof(uvTimer));
  uvTimer* timer = uvTimer::Unwrap(cdata);
  timer->Initialize();
  Handle<CData> handle(cdata);
  handle.SetWeakCallback(uvTimer::WeakCallback);
  return cdata;
}

static Value* luv_timer_start(uint32_t argc, Arguments& argv) {
  assert(argc && argv[0]->Is<CData>());
  return uvTimer::Unwrap(argv[0])->Start(argc, argv);
}

static Value* luv_timer_get_repeat(uint32_t argc, Arguments& argv) {
  assert(argc && argv[0]->Is<CData>());
  return uvTimer::Unwrap(argv[0])->GetRepeat(argc, argv);
}

static Value* luv_timer_set_repeat(uint32_t argc, Arguments& argv) {
  assert(argc && argv[0]->Is<CData>());
  return uvTimer::Unwrap(argv[0])->SetRepeat(argc, argv);
}

static Value* luv_timer_stop(uint32_t argc, Arguments& argv) {
  assert(argc && argv[0]->Is<CData>());
  return uvTimer::Unwrap(argv[0])->Stop(argc, argv);
}

void luv_timer_init(Object* uv) {
  Object* timer = Object::New();
  uv->Set("Timer", timer);
  timer->Set("create", Function::New(luv_create_timer));
  timer->Set("start", Function::New(luv_timer_start));
  timer->Set("getRepeat", Function::New(luv_timer_get_repeat));
  timer->Set("setRepeat", Function::New(luv_timer_set_repeat));
  timer->Set("stop", Function::New(luv_timer_stop));

}