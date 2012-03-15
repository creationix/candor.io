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


// Wrapper C functions when functions pointers are needed
static void luv_on_timer(uv_timer_t* handle, int status) {
  (reinterpret_cast<uvTimer*>(handle->data))->OnTimer(status);
}
static Value* luv_create_timer(uint32_t argc, Arguments& argv) {
  assert(argc == 0);
  return (new uvTimer())->Wrap();
}
static Value* luv_timer_start(uint32_t argc, Arguments& argv) {
  assert(argc && argv[0]->Is<CData>());
  return CWrapper::Unwrap<uvTimer>(argv[0])->Start(argc, argv);
}
static Value* luv_timer_stop(uint32_t argc, Arguments& argv) {
  assert(argc && argv[0]->Is<CData>());
  return CWrapper::Unwrap<uvTimer>(argv[0])->Stop(argc, argv);
}
static Value* luv_timer_again(uint32_t argc, Arguments& argv) {
  assert(argc && argv[0]->Is<CData>());
  return CWrapper::Unwrap<uvTimer>(argv[0])->Again(argc, argv);
}
static Value* luv_timer_get_repeat(uint32_t argc, Arguments& argv) {
  assert(argc && argv[0]->Is<CData>());
  return CWrapper::Unwrap<uvTimer>(argv[0])->GetRepeat(argc, argv);
}
static Value* luv_timer_set_repeat(uint32_t argc, Arguments& argv) {
  assert(argc && argv[0]->Is<CData>());
  return CWrapper::Unwrap<uvTimer>(argv[0])->SetRepeat(argc, argv);
}

// Create the Timer object that wraps the C functions
void luv_timer_init(Object* uv) {
  Object* timer = Object::New();
  uv->Set("Timer", timer);
  timer->Set("create", Function::New(luv_create_timer));
  timer->Set("start", Function::New(luv_timer_start));
  timer->Set("stop", Function::New(luv_timer_stop));
  timer->Set("again", Function::New(luv_timer_again));
  timer->Set("getRepeat", Function::New(luv_timer_get_repeat));
  timer->Set("setRepeat", Function::New(luv_timer_set_repeat));
}

// Implement class methods.

uvTimer::uvTimer() {
  uv_timer_init(uv_default_loop(), &handle);
  handle.data = this;
}

void uvTimer::OnTimer(int status) {
  onTimer->Call(0, NULL);
}

Value* uvTimer::Start(uint32_t argc, Arguments& argv) {
  assert(this == handle.data);
  assert(argc == 4 && argv[1]->Is<Number>() && argv[2]->Is<Number>() && argv[3]->Is<Function>());
  int64_t timeout = argv[1]->As<Number>()->IntegralValue();
  int64_t repeat = argv[2]->As<Number>()->IntegralValue();
  onTimer.Wrap(argv[3]->As<Function>());
  uv_timer_start(&handle, luv_on_timer, timeout, repeat);
  Ref();
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
  onTimer.Unwrap();
  Unref();
  return Nil::New();
}

Value* uvTimer::Again(uint32_t argc, Arguments& argv) {
  assert(argc == 1);
  uv_timer_again(&handle);
  return Nil::New();
}
