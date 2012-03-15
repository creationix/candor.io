#include <assert.h>
#include "candor.h"
#include "uv.h"
#include "luv.h"
#include "luv_handle.h"

using namespace candor;
using namespace candorIO;

// Wrapper C functions when functions pointers are needed
static void luv_on_close(uv_handle_t* handle) {
  (reinterpret_cast<uvHandle*>(handle->data))->OnClose();
}
static Value* luv_close(uint32_t argc, Arguments& argv) {
  assert(argc && argv[0]->Is<CData>());
  return CWrapper::Unwrap<uvHandle>(argv[0])->Close(argc, argv);
}

// Create the Handle object that wraps the C functions
void luv_handle_init(Object* uv) {
  Object* handle = Object::New();
  uv->Set("Handle", handle);
  handle->Set("close", Function::New(luv_close));
}

// Implement class methods.

void uvHandle::OnClose() {
  onClose->Call(0, NULL);
  onClose.Unwrap();
  Unref();
}

Value* uvHandle::Close(uint32_t argc, Arguments& argv) {
  if (argc > 1 && argv[1]->Is<Function>()) {
    onClose.Wrap(argv[1]->As<Function>());
  }
  uv_close(&handle, luv_on_close);
  Ref();
  return Nil::New();
}


