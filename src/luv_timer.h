#ifndef LUV_TIMER_H
#define LUV_TIMER_H

#include "luv_handle.h"
#include "candor.h"
using namespace candor;

namespace candorIO {

  class uvTimer : public uvHandle {
    uv_timer_t handle;
    Function* onTimer;
   public:
    uvTimer();
    void OnTimer(int status);
    Value* Start(uint32_t argc, Arguments& argv);
    Value* GetRepeat(uint32_t argc, Arguments& argv);
    Value* SetRepeat(uint32_t argc, Arguments& argv);
    Value* Stop(uint32_t argc, Arguments& argv);
    Value* Again(uint32_t argc, Arguments& argv);
  };
}

void luv_timer_init(Object* uv);

#endif