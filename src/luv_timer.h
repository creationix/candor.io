#ifndef LUV_TIMER_H
#define LUV_TIMER_H

#include "candor.h"

namespace candorIO {



  class uvTimer : public uvHandle {
    uv_timer_t handle;
    Function* onTimer;
   public:

    void Initialize();

    static void OnTimer(uv_timer_t* handle, int status);

    Value* Start(uint32_t argc, Arguments& argv);
    Value* GetRepeat(uint32_t argc, Arguments& argv);
    Value* SetRepeat(uint32_t argc, Arguments& argv);
    Value* Stop(uint32_t argc, Arguments& argv);
    Value* Again(uint32_t argc, Arguments& argv);

    static void WeakCallback(CData* cdata);
    static uvTimer* Unwrap(Value* value);
  };
}

using namespace candor;

void luv_timer_init(Object* uv);

#endif