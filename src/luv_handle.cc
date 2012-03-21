#include <assert.h>
#include "candor.h"
#include "uv.h"
#include "luv_handle.h"

using namespace candor;

static void luv_on_close(uv_handle_t* handle) {
  Object* obj = (Object*)handle->data;
  Value* callback = obj->Get("onTimer");
  if (callback->Is<Function>()) {
    callback->As<Function>()->Call(0, NULL);;
  }
}

static Value* luv_close(uint32_t argc, Arguments& argv) {
  assert(argc >= 1 && argc <= 2);
  Object* obj = argv[0]->As<Object>();
  uv_handle_t* handle = (uv_handle_t*)obj->Get("cdata")->As<CData>()->GetContents();
  if (argc == 2) {
    obj->Set("onClose", argv[1]->As<Function>());
  }
  uv_close(handle, luv_on_close);
  return Nil::New();
}

static Handle<Object> prototype;
Object* uv_handle_prototype() {
  if (!prototype.IsEmpty()) return *prototype;

  prototype.Wrap(Object::New());
  prototype->Set("close", Function::New(luv_close));
  return *prototype;
}

