#include "candor.h"
#include "uv.h"
#include "luv.h"
#include "luv_misc.h"

static Value* luv_last_error(uint32_t argc, Arguments& argv) {
  Object* error = Object::New();
  uv_err_t err = uv_last_error(uv_default_loop());
  error->Set("name", String::New(uv_err_name(err)));
  error->Set("error", String::New(uv_strerror(err)));
  return error;
}

void luv_misc_init(Object* uv) {
  uv->Set("lastError", Function::New(luv_last_error));
}
