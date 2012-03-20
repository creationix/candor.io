#include "luv.h"
#include "candor.h"
#include "uv.h"

#include <stdio.h>

using namespace candor;

static Value* luv_last_error(uint32_t argc, Arguments& argv) {
  Object* error = Object::New();
  uv_err_t err = uv_last_error(uv_default_loop());
  error->Set("name", String::New(uv_err_name(err)));
  error->Set("error", String::New(uv_strerror(err)));
  return error;
}

Value* uv_base_module(uint32_t argc, Arguments& argv) {
  Object* uv = Object::New();
  uv->Set("lastError", Function::New(luv_last_error));
  return uv;
}
