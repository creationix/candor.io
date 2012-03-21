#include "candor.h"
#include "uv.h"
#include "luv_stream.h"
#include "luv_tcp.h"

#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

using namespace candor;

/* Temporary hack: libuv should provide uv_inet_pton and uv_inet_ntop. */
#if defined(__MINGW32__) || defined(_MSC_VER)
# include <inet_net_pton.h>
# include <inet_ntop.h>
# define uv_inet_pton ares_inet_pton
# define uv_inet_ntop ares_inet_ntop

#else /* __POSIX__ */
# include <arpa/inet.h>
# define uv_inet_pton inet_pton
# define uv_inet_ntop inet_ntop
#endif


using namespace candor;

static Value* luv_create_tcp(uint32_t argc, Arguments& argv) {
  assert(argc == 0);
  Object* obj = uv_tcp_prototype()->Clone();
  CData* cdata = CData::New(sizeof(uv_tcp_t));
  uv_tcp_t* handle = (uv_tcp_t*)cdata->GetContents();
  handle->data = new Handle<Object>(obj);
  obj->Set("cdata", cdata);
  uv_tcp_init(uv_default_loop(), handle);
  return obj;
}

static Value* luv_tcp_nodelay(uint32_t argc, Arguments& argv) {
  assert(argc == 2);
  Object* obj = argv[0]->As<Object>();
  uv_tcp_t* handle = (uv_tcp_t*)obj->Get("cdata")->As<CData>()->GetContents();
  int nodelay = argv[1]->ToBoolean()->IsTrue();
  int status = uv_tcp_nodelay(handle, nodelay);
  return Number::NewIntegral(status);
}

static Value* luv_tcp_keepalive(uint32_t argc, Arguments& argv) {
  assert(argc == 3);
  Object* obj = argv[0]->As<Object>();
  uv_tcp_t* handle = (uv_tcp_t*)obj->Get("cdata")->As<CData>()->GetContents();
  int keepalive = argv[1]->ToBoolean()->IsTrue();
  unsigned int delay = argv[2]->ToNumber()->IntegralValue();
  int status = uv_tcp_keepalive(handle, keepalive, delay);
  return Number::NewIntegral(status);
}

static Value* luv_tcp_bind(uint32_t argc, Arguments& argv) {
  assert(argc == 3);
  Object* obj = argv[0]->As<Object>();
  uv_tcp_t* handle = (uv_tcp_t*)obj->Get("cdata")->As<CData>()->GetContents();
  const char* host = argv[1]->ToString()->Value();
  int port = argv[2]->ToNumber()->IntegralValue();
  int status = uv_tcp_bind(handle, uv_ip4_addr(host, port));
  return Number::NewIntegral(status);
}

static Value* luv_tcp_getsockname(uint32_t argc, Arguments& argv) {
  assert(argc == 1);
  Object* obj = argv[0]->As<Object>();
  uv_tcp_t* handle = (uv_tcp_t*)obj->Get("cdata")->As<CData>()->GetContents();
  int port = 0;
  char ip[INET6_ADDRSTRLEN];
  int family;
  struct sockaddr_storage address;
  int addrlen = sizeof(address);
  int status = uv_tcp_getsockname(handle, (struct sockaddr*)(&address), &addrlen);
  if (status) return Number::NewIntegral(status);
  family = address.ss_family;
  if (family == AF_INET) {
    struct sockaddr_in* addrin = (struct sockaddr_in*)&address;
    uv_inet_ntop(AF_INET, &(addrin->sin_addr), ip, INET6_ADDRSTRLEN);
    port = ntohs(addrin->sin_port);
  } else if (family == AF_INET6) {
    struct sockaddr_in6* addrin6 = (struct sockaddr_in6*)&address;
    uv_inet_ntop(AF_INET6, &(addrin6->sin6_addr), ip, INET6_ADDRSTRLEN);
    port = ntohs(addrin6->sin6_port);
  }
  Object* result = Object::New();
  result->Set("port", Number::NewIntegral(port));
  result->Set("address", String::New(ip));
  return result;
}

static Value* luv_tcp_getpeername(uint32_t argc, Arguments& argv) {
  assert(argc == 1);
  Object* obj = argv[0]->As<Object>();
  uv_tcp_t* handle = (uv_tcp_t*)obj->Get("cdata")->As<CData>()->GetContents();
  int port = 0;
  char ip[INET6_ADDRSTRLEN];
  int family;
  struct sockaddr_storage address;
  int addrlen = sizeof(address);
  int status = uv_tcp_getpeername(handle, (struct sockaddr*)(&address), &addrlen);
  if (status) return Number::NewIntegral(status);
  family = address.ss_family;
  if (family == AF_INET) {
    struct sockaddr_in* addrin = (struct sockaddr_in*)&address;
    uv_inet_ntop(AF_INET, &(addrin->sin_addr), ip, INET6_ADDRSTRLEN);
    port = ntohs(addrin->sin_port);
  } else if (family == AF_INET6) {
    struct sockaddr_in6* addrin6 = (struct sockaddr_in6*)&address;
    uv_inet_ntop(AF_INET6, &(addrin6->sin6_addr), ip, INET6_ADDRSTRLEN);
    port = ntohs(addrin6->sin6_port);
  }
  Object* result = Object::New();
  result->Set("port", Number::NewIntegral(port));
  result->Set("address", String::New(ip));
  return result;
}

static void luv_on_connect(uv_connect_t* req, int status) {
  Object* obj = **((Handle<Object>*)req->data);
  Value* callback = obj->Get("onConnect");
  if (callback->Is<Function>()) {
    Value* argv[1];
    argv[0] = Number::NewIntegral(status);
    callback->As<Function>()->Call(1, argv);
  }
  free(req);
}

static Value* luv_tcp_connect(uint32_t argc, Arguments& argv) {
  assert(argc >= 3 && argc <= 4);
  Object* obj = argv[0]->As<Object>();
  uv_tcp_t* handle = (uv_tcp_t*)obj->Get("cdata")->As<CData>()->GetContents();
  const char* host = argv[1]->ToString()->Value();
  int port = argv[2]->ToNumber()->IntegralValue();
  if (argc == 4) {
    obj->Set("onConnect", argv[3]->As<Function>());
  }
  struct sockaddr_in address = uv_ip4_addr(host, port);
  uv_connect_t* req = (uv_connect_t*)malloc(sizeof(uv_connect_t));
  req->data = obj;
  int status = uv_tcp_connect(req, handle, address, luv_on_connect);
  return Number::NewIntegral(status);
}

static Handle<Object> module;
Object* uv_tcp_module() {
  if (!module.IsEmpty()) return *module;
  module.Wrap(Object::New());
  module->Set("create", Function::New(luv_create_tcp));
  return *module;
}

static Handle<Object> prototype;
Object* uv_tcp_prototype() {
  if (!prototype.IsEmpty()) return *prototype;
  prototype.Wrap(uv_stream_prototype()->Clone());
  prototype->Set("nodelay", Function::New(luv_tcp_nodelay));
  prototype->Set("keepalive", Function::New(luv_tcp_keepalive));
  prototype->Set("bind", Function::New(luv_tcp_bind));
  prototype->Set("getsockname", Function::New(luv_tcp_getsockname));
  prototype->Set("getpeername", Function::New(luv_tcp_getpeername));
  prototype->Set("connect", Function::New(luv_tcp_connect));
  return *prototype;
}


