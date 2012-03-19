#include "luv.h"
#include "candor.h"
#include "uv.h"
#include "luv_misc.h"
#include "luv_timer.h"
#include "luv_tcp.h"

using namespace candor;

void luv_init(Object* global) {
  // Create a global args array.
  Object* uv = Object::New();
  global->Set("uv", uv);

  luv_timer_init(uv);
  luv_tcp_init(uv);

}
