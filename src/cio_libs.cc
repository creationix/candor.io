#include "cio_libs.h"

#include "cio.h"    // cio_global_context
#include "candor.h"

#include <stdio.h>

using namespace candor;


#define XX(name)                                \
extern const char __binding_lib_##name[];      \
static Handle<Object> module_##name;            \
Object* cio_##name##_module() {                 \
  if (!module_##name.IsEmpty()) {               \
    return *module_##name;                      \
  }                                             \
  Function* fn = Function::New(                 \
      #name, __binding_lib_##name);           \
  fn->SetContext(cio_global_context());         \
  module_##name.Wrap(fn->Call(0, NULL));        \
  return *module_##name;                        \
}                                               \

CIO_LIB_MAP(XX)
#undef XX

