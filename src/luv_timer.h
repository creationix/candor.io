#ifndef LUV_TIMER_H
#define LUV_TIMER_H

#include "candor.h"
using namespace candor;

namespace candorIO {

  class uvTimer : public CWrapper {
    uv_timer_t handle;
    Handle<Function> onTimer;
    Handle<Function> onClose;
   public:
    uvTimer();

    // Timer methods
    void OnTimer(int status);
    Value* Start(uint32_t argc, Arguments& argv);
    Value* GetRepeat(uint32_t argc, Arguments& argv);
    Value* SetRepeat(uint32_t argc, Arguments& argv);
    Value* Stop(uint32_t argc, Arguments& argv);
    Value* Again(uint32_t argc, Arguments& argv);

    // Handle methods
    void OnClose();
    Value* Close(uint32_t argc, Arguments& argv);
  };
}

Value* uv_timer_module(uint32_t argc, Arguments& argv);

#endif