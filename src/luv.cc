#include "luv.h"

#include "candor.h"
#include "uv.h"

#include <assert.h> // assert

using namespace candor;

static Value* luv_last_error(uint32_t argc, Arguments& argv) {
  assert(argc == 0);
  Object* error = Object::New();
  uv_err_t err = uv_last_error(uv_default_loop());
  error->Set("name", String::New(uv_err_name(err)));
  error->Set("error", String::New(uv_strerror(err)));
  return error;
}

static Handle<Object> module;
Object* uv_base_module() {
  if (!module.IsEmpty()) return *module;
  module.Wrap(Object::New());
  module->Set("lastError", Function::New(luv_last_error));
  return *module;
}
