#ifndef LUV_HANDLE_H
#define LUV_HANDLE_H

#include "candor.h"
using namespace candor;

namespace candorIO {

  class uvHandle : public CWrapper {
    uv_handle_t handle;
    Handle<Function> onClose;
   public:
    void OnClose();
    Value* Close(uint32_t argc, Arguments& argv);
  };
}

void luv_handle_init(Object* uv);

#endif