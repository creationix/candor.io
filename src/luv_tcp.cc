#include "candor.h"
#include "uv.h"
#include "luv.h"
#include "luv_tcp.h"

#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

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
using namespace candorIO;

static Value* luv_create_tcp(uint32_t argc, Arguments& argv) {
  assert(argc == 0);
  return (new uvTcp())->Wrap();
}

static Value* luv_tcp_nodelay(uint32_t argc, Arguments& argv) {
  assert(argc && argv[0]->Is<CData>());
  return CWrapper::Unwrap<uvTcp>(argv[0])->Nodelay(argc, argv);
}
static Value* luv_tcp_keepalive(uint32_t argc, Arguments& argv) {
  assert(argc && argv[0]->Is<CData>());
  return CWrapper::Unwrap<uvTcp>(argv[0])->Keepalive(argc, argv);
}
static Value* luv_tcp_bind(uint32_t argc, Arguments& argv) {
  assert(argc && argv[0]->Is<CData>());
  return CWrapper::Unwrap<uvTcp>(argv[0])->Bind(argc, argv);
}
static Value* luv_tcp_getsockname(uint32_t argc, Arguments& argv) {
  assert(argc && argv[0]->Is<CData>());
  return CWrapper::Unwrap<uvTcp>(argv[0])->Getsockname(argc, argv);
}
static Value* luv_tcp_getpeername(uint32_t argc, Arguments& argv) {
  assert(argc && argv[0]->Is<CData>());
  return CWrapper::Unwrap<uvTcp>(argv[0])->Getpeername(argc, argv);
}
static void luv_on_connect(uv_connect_t* req, int status) {
  (reinterpret_cast<uvTcp*>(req->data))->OnConnect(status);
}
static Value* luv_tcp_connect(uint32_t argc, Arguments& argv) {
  assert(argc && argv[0]->Is<CData>());
  return CWrapper::Unwrap<uvTcp>(argv[0])->Connect(argc, argv);
}

static void luv_on_shutdown(uv_shutdown_t* req, int status) {
  (reinterpret_cast<uvTcp*>(req->data))->OnShutdown(status);
}
static Value* luv_shutdown(uint32_t argc, Arguments& argv) {
  assert(argc && argv[0]->Is<CData>());
  return CWrapper::Unwrap<uvTcp>(argv[0])->Shutdown(argc, argv);
}
static void luv_on_connection(uv_stream_t* server, int status) {
  (reinterpret_cast<uvTcp*>(server->data))->OnConnection(status);
}
static Value* luv_listen(uint32_t argc, Arguments& argv) {
  assert(argc && argv[0]->Is<CData>());
  return CWrapper::Unwrap<uvTcp>(argv[0])->Listen(argc, argv);
}
static Value* luv_accept(uint32_t argc, Arguments& argv) {
  assert(argc && argv[0]->Is<CData>());
  return CWrapper::Unwrap<uvTcp>(argv[0])->Accept(argc, argv);
}
static uv_buf_t luv_on_alloc(uv_handle_t* handle, size_t suggested_size) {
  uv_buf_t buf;
  buf.base = (char*)malloc(suggested_size);
  buf.len = suggested_size;
  return buf;
}
static void luv_on_read(uv_stream_t* stream, ssize_t nread, uv_buf_t buf) {
  (reinterpret_cast<uvTcp*>(stream->data))->OnRead(nread, buf);
}
static Value* luv_read_start(uint32_t argc, Arguments& argv) {
  assert(argc && argv[0]->Is<CData>());
  return CWrapper::Unwrap<uvTcp>(argv[0])->ReadStart(argc, argv);
}
static Value* luv_read_stop(uint32_t argc, Arguments& argv) {
  assert(argc && argv[0]->Is<CData>());
  return CWrapper::Unwrap<uvTcp>(argv[0])->ReadStop(argc, argv);
}
static void luv_on_write(uv_write_t* req, int status) {
  (reinterpret_cast<uvTcp*>(req->data))->OnWrite(status);
}
static Value* luv_write(uint32_t argc, Arguments& argv) {
  assert(argc && argv[0]->Is<CData>());
  return CWrapper::Unwrap<uvTcp>(argv[0])->Write(argc, argv);
}
static Value* luv_is_readable(uint32_t argc, Arguments& argv) {
  assert(argc && argv[0]->Is<CData>());
  return CWrapper::Unwrap<uvTcp>(argv[0])->IsReadable(argc, argv);
}
static Value* luv_is_writable(uint32_t argc, Arguments& argv) {
  assert(argc && argv[0]->Is<CData>());
  return CWrapper::Unwrap<uvTcp>(argv[0])->IsWritable(argc, argv);
}

static void luv_on_close(uv_handle_t* handle) {
  (reinterpret_cast<uvTcp*>(handle->data))->OnClose();
}
static Value* luv_close(uint32_t argc, Arguments& argv) {
  assert(argc && argv[0]->Is<CData>());
  return CWrapper::Unwrap<uvTcp>(argv[0])->Close(argc, argv);
}

// Create the Timer object that wraps the C functions
void luv_tcp_init(Object* uv) {
  Object* tcp = Object::New();
  uv->Set("Tcp", tcp);
  tcp->Set("create", Function::New(luv_create_tcp));

  tcp->Set("nodelay", Function::New(luv_tcp_nodelay));
  tcp->Set("keepalive", Function::New(luv_tcp_keepalive));
  tcp->Set("bind", Function::New(luv_tcp_bind));
  tcp->Set("getsockname", Function::New(luv_tcp_getsockname));
  tcp->Set("getpeername", Function::New(luv_tcp_getpeername));
  tcp->Set("connect", Function::New(luv_tcp_connect));

  tcp->Set("shutdown", Function::New(luv_shutdown));
  tcp->Set("listen", Function::New(luv_listen));
  tcp->Set("accept", Function::New(luv_accept));
  tcp->Set("readStart", Function::New(luv_read_start));
  tcp->Set("readStop", Function::New(luv_read_stop));
  tcp->Set("write", Function::New(luv_write));
  tcp->Set("isReadable", Function::New(luv_is_readable));
  tcp->Set("isWritable", Function::New(luv_is_writable));

  tcp->Set("close", Function::New(luv_close));
}

// Implement class methods.

uvTcp::uvTcp() {
  uv_tcp_init(uv_default_loop(), &handle);
  handle.data = this;
}

Value* uvTcp::Nodelay(uint32_t argc, Arguments& argv) {
  assert(argc == 2 && argv[1]->Is<Boolean>());
  int nodelay = argv[1]->ToBoolean()->IsTrue();
  int status = uv_tcp_nodelay(&handle, nodelay);
  return Number::NewIntegral(status);
}

Value* uvTcp::Keepalive(uint32_t argc, Arguments& argv) {
  assert(argc == 3 && argv[1]->Is<Boolean>() && argv[2]->Is<Number>());
  int keepalive = argv[1]->ToBoolean()->IsTrue();
  unsigned int delay = argv[2]->ToNumber()->IntegralValue();
  int status = uv_tcp_keepalive(&handle, keepalive, delay);
  return Number::NewIntegral(status);
}

Value* uvTcp::Bind(uint32_t argc, Arguments& argv) {
  assert(argc == 3 && argv[1]->Is<String>() && argv[2]->Is<Number>());
  const char* host = argv[1]->ToString()->Value();
  int port = argv[2]->ToNumber()->IntegralValue();
  struct sockaddr_in address = uv_ip4_addr(host, port);
  int status = uv_tcp_bind(&handle, address);
  return Number::NewIntegral(status);
}

Value* uvTcp::Getsockname(uint32_t argc, Arguments& argv) {
  assert(argc == 1);
  int port = 0;
  char ip[INET6_ADDRSTRLEN];
  int family;

  struct sockaddr_storage address;
  int addrlen = sizeof(address);

  int status = uv_tcp_getsockname(&handle, (struct sockaddr*)(&address), &addrlen);
  if (status) {
    return Number::NewIntegral(status);
  }

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

  Object* obj = Object::New();
  obj->Set("port", Number::NewIntegral(port));
  obj->Set("address", String::New(ip));

  return obj;
}

Value* uvTcp::Getpeername(uint32_t argc, Arguments& argv) {
  assert(argc == 1);
  int port = 0;
  char ip[INET6_ADDRSTRLEN];
  int family;

  struct sockaddr_storage address;
  int addrlen = sizeof(address);

  int status = uv_tcp_getpeername(&handle, (struct sockaddr*)(&address), &addrlen);
  if (status) {
    return Number::NewIntegral(status);
  }

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

  Object* obj = Object::New();
  obj->Set("port", Number::NewIntegral(port));
  obj->Set("address", String::New(ip));

  return obj;
}

void uvTcp::OnConnect(int status) {
  onConnect->Call(0, NULL);
  onConnect.Unwrap();
  Unref();
}

Value* uvTcp::Connect(uint32_t argc, Arguments& argv) {
  assert(argc == 4 && argv[1]->Is<String>() && argv[2]->Is<Number>() && argv[3]->Is<Function>());
  const char* host = argv[1]->ToString()->Value();
  int port = argv[2]->ToNumber()->IntegralValue();
  onClose.Wrap(argv[3]->As<Function>());
  struct sockaddr_in address = uv_ip4_addr(host, port);
  uv_connect_t* req = (uv_connect_t*)malloc(sizeof(uv_connect_t));
  req->data = this;
  int status = uv_tcp_connect(req, &handle, address, luv_on_connect);
  Ref();
  return Number::NewIntegral(status);
}

void uvTcp::OnShutdown(int status) {
  Value* argv[1];
  argv[0] = Number::NewIntegral(status);
  onShutdown->Call(1, argv);
  onShutdown.Unwrap();
  Unref();
}

Value* uvTcp::Shutdown(uint32_t argc, Arguments& argv) {
  if (argc > 1 && argv[1]->Is<Function>()) {
    onShutdown.Wrap(argv[1]->As<Function>());
  }
  uv_shutdown_t* req = (uv_shutdown_t*)malloc(sizeof(uv_shutdown_t));
  req->data = this;
  int status = uv_shutdown(req, (uv_stream_t*)&handle, luv_on_shutdown);
  Ref();
  return Number::NewIntegral(status);
}

void uvTcp::OnConnection(int status) {
  Value* argv[1];
  argv[0] = Number::NewIntegral(status);
  onConnection->Call(1, argv);
  // TODO: unref and unwrap, but not here, this is repeating
}

Value* uvTcp::Listen(uint32_t argc, Arguments& argv) {
  assert(argc == 3 && argv[1]->Is<Number>() && argv[2]->Is<Function>());
  onConnection.Wrap(argv[2]->As<Function>());
  int backlog = argv[1]->ToNumber()->IntegralValue();
  int status = uv_listen((uv_stream_t*)&handle, backlog, luv_on_connection);
  Ref();
  return Number::NewIntegral(status);
}

Value* uvTcp::Accept(uint32_t argc, Arguments& argv) {
  assert(argc == 2 && argv[1]->Is<CData>());
  uvTcp* client = CWrapper::Unwrap<uvTcp>(argv[1]);
  int status = uv_accept((uv_stream_t*)&handle, (uv_stream_t*)&(client->handle));
  return Number::NewIntegral(status);
}

void uvTcp::OnRead(ssize_t nread, uv_buf_t buf) {
  Value* argv[2];
  argv[0] = Number::NewIntegral(nread);
  if (nread > 0) {
    argv[1] = String::New(buf.base, nread);
  } else {
    argv[1] = Nil::New();
  }
  onRead->Call(2, argv);
  free(buf.base);
}

Value* uvTcp::ReadStart(uint32_t argc, Arguments& argv) {
  assert(argc == 2 && argv[1]->Is<Function>());
  onRead.Wrap(argv[1]->As<Function>());
  int status = uv_read_start((uv_stream_t*)&handle, luv_on_alloc, luv_on_read);
  Ref();
  return Number::NewIntegral(status);
}

Value* uvTcp::ReadStop(uint32_t argc, Arguments& argv) {
  assert(argc == 1);
  int status = uv_read_stop((uv_stream_t*)&handle);
  onRead.Unwrap();
  Unref();
  return Number::NewIntegral(status);
}

void uvTcp::OnWrite(int status) {
  printf("TODO: Implement uvTcp::OnWrite\n");
  // TODO call write callback if there is one
  // Value* argv[1];
  // argv[0] = Number::NewIntegral(status);
  // onWrite->Call(1, argv);
  // memory managent probably needs to happen too.
}

Value* uvTcp::Write(uint32_t argc, Arguments& argv) {
  assert(argc >= 2 && argv[1]->Is<String>());
  String* str = argv[1]->As<String>();
  uv_buf_t* buf = (uv_buf_t*)malloc(sizeof(uv_buf_t));
  uint32_t len = str->Length();
  const char* chunk = str->Value();
  buf->len = len;
  buf->base = (char*)chunk;
  // strncpy(buf->base, str->Value(), buf->len);
  if (argc > 2 && argv[2]->Is<Function>()) {
    // Store callback somehow    
  }
  uv_write_t* req = (uv_write_t*)malloc(sizeof(uv_write_t));
  req->data = this;
  int status = uv_write(req, (uv_stream_t*)&handle, buf, 1, luv_on_write);
  return Number::NewIntegral(status);
}

Value* uvTcp::IsReadable(uint32_t argc, Arguments& argv) {
  return uv_is_readable((uv_stream_t*)&handle) ?
    Boolean::True() :
    Boolean::False();
}

Value* uvTcp::IsWritable(uint32_t argc, Arguments& argv) {
  return uv_is_writable((uv_stream_t*)&handle) ?
    Boolean::True() :
    Boolean::False();
}

void uvTcp::OnClose() {
  onClose->Call(0, NULL);
  onClose.Unwrap();
  Unref();
}

Value* uvTcp::Close(uint32_t argc, Arguments& argv) {
  if (argc > 1 && argv[1]->Is<Function>()) {
    onClose.Wrap(argv[1]->As<Function>());
  }
  uv_close((uv_handle_t*)&handle, luv_on_close);
  Ref();
  return Nil::New();
}
