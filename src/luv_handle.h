#ifndef LUV_HANDLE_H
#define LUV_HANDLE_H

#include "candor.h"
using namespace candor;

namespace candorIO {

  class uvHandle {
    uv_handle_t handle;
    Function* onClose;
   public:

    static void OnClose(uv_handle_t* handle);

    Value* Close(uint32_t argc, Arguments& argv);

    static uvHandle* Unwrap(Value* value);

  };
}

void luv_handle_init(Object* uv);

#endif