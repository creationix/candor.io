#include "luv.h"

#include "candor.h"
#include "uv.h"

using namespace candor;

template uv_handle_t* UVData::ObjectTo<uv_handle_t>(candor::Object* obj);
template uv_timer_t* UVData::ObjectTo<uv_timer_t>(candor::Object* obj);

UVData::UVData(size_t size, Object* prototype) {
  // Create a new data object
  if (prototype) {
    obj.Wrap(prototype->Clone());
  } else {
    obj.Wrap(Object::New());
  }
  // Give it a cdata pointer to us
  obj->Set("cdata", Wrap());
  // And Make it weak.  We need to Ref and Unref for pending callbacks.
  obj.Unref();

  // Create a uv structure
  handle = (uv_handle_t*)new char[size];
  // And point it to us too.
  handle->data = this;
}

UVData::~UVData() {
  // Free the uv struct
  delete handle;
  // Let go of the data object
  obj.Unwrap();
}

template <class T>
T* UVData::ObjectTo(candor::Object* obj) {
  uv_handle_t* handle = CWrapper::Unwrap<UVData>(obj->Get("cdata"))->handle;
  return (T*)handle;
}

Object* UVData::VoidToObject(void* ptr) {
  return *((UVData*)ptr)->obj;
}
