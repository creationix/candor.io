#include "cio_libs.h"

#include "cio.h"    // cio_global_context
#include "candor.h"

#include <stdio.h>

using namespace candor;


#define XX(name)                                \
extern char _binary_lib_##name##_can_start;     \
extern char _binary_lib_##name##_can_end;       \
static Handle<Object> module_##name;            \
Object* cio_##name##_module() {                 \
  if (!module_##name.IsEmpty()) {               \
    return *module_##name;                      \
  }                                             \
  char* code = &_binary_lib_##name##_can_start; \
  int len = &_binary_lib_##name##_can_end -     \
            &_binary_lib_##name##_can_start;    \
  Function* fn = Function::New(#name, code, len);\
  fn->SetContext(cio_global_context());         \
  module_##name.Wrap(fn->Call(0, NULL));        \
  return *module_##name;                        \
}                                               \

CIO_LIB_MAP(XX)
#undef XX

