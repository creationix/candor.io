#ifndef LUV_TCP_H
#define LUV_TCP_H

#include "candor.h"
using namespace candor;

namespace candorIO {

  class uvTcp : public CWrapper {
    uv_tcp_t handle;
    Handle<Function> onConnect;
    Handle<Function> onShutdown;
    Handle<Function> onConnection;
    Handle<Function> onRead;
    Handle<Function> onClose;
   public:
    uvTcp();

    // Tcp methods
    Value* Nodelay(uint32_t argc, Arguments& argv);
    Value* Keepalive(uint32_t argc, Arguments& argv);
    Value* Bind(uint32_t argc, Arguments& argv);
    Value* Getsockname(uint32_t argc, Arguments& argv);
    Value* Getpeername(uint32_t argc, Arguments& argv);
    void OnConnect(int status);
    Value* Connect(uint32_t argc, Arguments& argv);

    // Stream methods
    void OnShutdown(int status);
    Value* Shutdown(uint32_t argc, Arguments& argv);
    void OnConnection(int status);
    Value* Listen(uint32_t argc, Arguments& argv);
    Value* Accept(uint32_t argc, Arguments& argv);
    void OnRead(ssize_t nread, uv_buf_t buf);
    Value* ReadStart(uint32_t argc, Arguments& argv);
    Value* ReadStop(uint32_t argc, Arguments& argv);
    void OnWrite(int status);
    Value* Write(uint32_t argc, Arguments& argv);
    Value* IsReadable(uint32_t argc, Arguments& argv);
    Value* IsWritable(uint32_t argc, Arguments& argv);

    // Handle methods
    void OnClose();
    Value* Close(uint32_t argc, Arguments& argv);
  };
}

void luv_tcp_init(Object* uv);

#endif