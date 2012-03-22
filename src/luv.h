#ifndef LUV_H
#define LUV_H

#include "candor.h"
#include "uv.h"

class UVData: public candor::CWrapper {
 public:
  uv_handle_t* handle;
  candor::Handle<candor::Object> obj;
  UVData(size_t size, candor::Object* prototype);
  ~UVData();
  // Meant to be used on the void* data property of uv handles and requests
  static candor::Object* VoidToObject(void* v);
  // useful for extracting the uv_handle_t out of data objects
  template <class T>
  static T* ObjectTo(candor::Object* obj);
};

#endif 
