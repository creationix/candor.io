#include "candor.h"
#include "uv.h"
#include <string.h>

using namespace candor;

int main(int argc, char** argv) {
  Isolate i;
  const char* code = "a = { hello: 'world' }\n";
  Function* f = Function::New(&i, code, strlen(code));
  // Value* result = 
  f->Call(NULL, 0, NULL);

  uv_run(uv_default_loop());

  return 0;
}