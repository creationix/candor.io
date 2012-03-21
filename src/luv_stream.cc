#include "candor.h"
#include "uv.h"
#include "luv_handle.h"
#include "luv_stream.h"

#include <assert.h>
#include <stdlib.h>

using namespace candor;

// TODO: remove when candor API has this.
static Object* CloneObject(Object* original) {
  Object* clone = Object::New();
  Array* keys = original->Keys();
  int64_t i = keys->Length();
  while (i--) {
    Value* key = keys->Get(i);
    clone->Set(key, original->Get(key));
  }
  return clone;
}

static void luv_on_shutdown(uv_shutdown_t* req, int status) {
  Object* obj = (Object*)req->data;
  Value* callback = obj->Get("onShutdown");
  if (callback->Is<Function>()) {
    Value* argv[1];
    argv[0] = Number::NewIntegral(status);
    callback->As<Function>()->Call(1, argv);
  }
  free(req);
}

static Value* luv_shutdown(uint32_t argc, Arguments& argv) {
  assert(argc >= 1 && argc <= 2);
  Object* obj = argv[0]->As<Object>();
  uv_stream_t* handle = (uv_stream_t*)obj->Get("cdata")->As<CData>()->GetContents();
  if (argc == 2) {
    obj->Set("onShutdown", argv[1]->As<Function>());
  }
  uv_shutdown_t* req = (uv_shutdown_t*)malloc(sizeof(uv_shutdown_t));
  req->data = obj;
  int status = uv_shutdown(req, handle, luv_on_shutdown);
  return Number::NewIntegral(status);
}

static void luv_on_connection(uv_stream_t* server, int status) {
  Object* obj = (Object*)server->data;
  Value* callback = obj->Get("onConnection");
  if (callback->Is<Function>()) {
    Value* argv[1];
    argv[0] = Number::NewIntegral(status);
    callback->As<Function>()->Call(1, argv);
  }
}

static Value* luv_listen(uint32_t argc, Arguments& argv) {
  assert(argc >= 2 && argc <= 3);
  Object* obj = argv[0]->As<Object>();
  uv_stream_t* handle = (uv_stream_t*)obj->Get("cdata")->As<CData>()->GetContents();
  int backlog = argv[1]->ToNumber()->IntegralValue();
  if (argc == 3) {
    obj->Set("onConnection", argv[2]->As<Function>());
  }
  int status = uv_listen(handle, backlog, luv_on_connection);
  return Number::NewIntegral(status);
}

static Value* luv_accept(uint32_t argc, Arguments& argv) {
  assert(argc == 2);
  Object* obj = argv[0]->As<Object>();
  uv_stream_t* handle = (uv_stream_t*)obj->Get("cdata")->As<CData>()->GetContents();
  Object* obj2 = argv[1]->As<Object>();
  uv_stream_t* client = (uv_stream_t*)obj2->Get("cdata")->As<CData>()->GetContents();
  int status = uv_accept(handle, client);
  return Number::NewIntegral(status);
}

static uv_buf_t luv_on_alloc(uv_handle_t* handle, size_t suggested_size) {
  uv_buf_t buf;
  buf.base = (char*)malloc(suggested_size);
  buf.len = suggested_size;
  return buf;
}

static void luv_on_read(uv_stream_t* stream, ssize_t nread, uv_buf_t buf) {
  Object* obj = (Object*)stream->data;
  Value* callback = obj->Get("onRead");
  if (callback->Is<Function>()) {
    Value* argv[2];
    argv[0] = Number::NewIntegral(nread);
    if (nread > 0) {
      argv[1] = String::New(buf.base, nread);
    } else {
      argv[1] = Nil::New();
    }
    callback->As<Function>()->Call(2, argv);
  }
  free(buf.base);
}

static Value* luv_read_start(uint32_t argc, Arguments& argv) {
  assert(argc >= 1 && argc <= 2);
  Object* obj = argv[0]->As<Object>();
  uv_stream_t* handle = (uv_stream_t*)obj->Get("cdata")->As<CData>()->GetContents();
  if (argc == 2) {
    obj->Set("onRead", argv[1]->As<Function>());
  }
  int status = uv_read_start(handle, luv_on_alloc, luv_on_read);
  return Number::NewIntegral(status);
}

static Value* luv_read_stop(uint32_t argc, Arguments& argv) {
  assert(argc == 1);
  Object* obj = argv[0]->As<Object>();
  uv_stream_t* handle = (uv_stream_t*)obj->Get("cdata")->As<CData>()->GetContents();
  int status = uv_read_stop(handle);
  return Number::NewIntegral(status);
}

static void luv_on_write(uv_write_t* req, int status) {
  // TODO call write callback if there is one
  // Value* argv[1];
  // argv[0] = Number::NewIntegral(status);
  // onWrite->Call(1, argv);
  // memory managent probably needs to happen too.
}

static Value* luv_write(uint32_t argc, Arguments& argv) {
  assert(argc >= 2 && argc <= 3);
  Object* obj = argv[0]->As<Object>();
  uv_stream_t* handle = (uv_stream_t*)obj->Get("cdata")->As<CData>()->GetContents();
  String* str = argv[1]->As<String>();
  uv_buf_t* buf = (uv_buf_t*)malloc(sizeof(uv_buf_t));
  uint32_t len = str->Length();
  const char* chunk = str->Value();
  buf->len = len;
  buf->base = (char*)chunk;
  if (argc == 3) {
    assert(argv[2]->Is<Function>());
    // TODO: store callback somehow;
  }
  uv_write_t* req = (uv_write_t*)malloc(sizeof(uv_write_t));
  req->data = obj;
  int status = uv_write(req, handle, buf, 1, luv_on_write);
  return Number::NewIntegral(status);
}

static Value* luv_is_readable(uint32_t argc, Arguments& argv) {
  assert(argc == 1);
  Object* obj = argv[0]->As<Object>();
  uv_stream_t* handle = (uv_stream_t*)obj->Get("cdata")->As<CData>()->GetContents();
  return uv_is_readable(handle) ?
    Boolean::True() :
    Boolean::False();
}

static Value* luv_is_writable(uint32_t argc, Arguments& argv) {
  assert(argc == 1);
  Object* obj = argv[0]->As<Object>();
  uv_stream_t* handle = (uv_stream_t*)obj->Get("cdata")->As<CData>()->GetContents();
  return uv_is_writable(handle) ?
    Boolean::True() :
    Boolean::False();
}

static Object* prototype;
Object* uv_stream_prototype() {
  if (prototype) return prototype;
  prototype = CloneObject(uv_handle_prototype());
  new Handle<Object>(prototype);
  prototype->Set("shutdown", Function::New(luv_shutdown));
  prototype->Set("listen", Function::New(luv_listen));
  prototype->Set("accept", Function::New(luv_accept));
  prototype->Set("readStart", Function::New(luv_read_start));
  prototype->Set("readStop", Function::New(luv_read_stop));
  prototype->Set("write", Function::New(luv_write));
  prototype->Set("isReadable", Function::New(luv_is_readable));
  prototype->Set("isWritable", Function::New(luv_is_writable));
  return prototype;
}

