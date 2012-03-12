#include <assert.h>
#include "candor.h"
#include "uv.h"
#include "luv.h"
#include "luv_handle.h"

using namespace candor;
using namespace candorIO;

static void luv_on_close(uv_handle_t* handle);

void uvHandle::OnClose() {
  Unref();
  onClose->Call(0, NULL);
}

Value* uvHandle::Close(uint32_t argc, Arguments& argv) {
  if (argc > 1 && argv[1]->Is<Function>()) {
    onClose = argv[1]->As<Function>();
  }
  uv_close(&handle, luv_on_close);
  Ref();
  return Nil::New();
}

static void luv_on_close(uv_handle_t* handle) {
  (reinterpret_cast<uvHandle*>(handle->data))->OnClose();
}

static Value* luv_close(uint32_t argc, Arguments& argv) {
  assert(argc && argv[0]->Is<CData>());
  return CWrapper::Unwrap<uvHandle>(argv[0])->Close(argc, argv);
}

void luv_handle_init(Object* uv) {
  Object* handle = Object::New();
  uv->Set(String::New("Handle", 6), handle);
  handle->Set(String::New("close", 5), Function::New(luv_close));
}
