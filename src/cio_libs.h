#ifndef CIO_LIBS_H
#define CIO_LIBS_H

#include "candor.h"

#define CIO_LIB_MAP(XX)  \
  XX(net)

#define XX(name) candor::Object* cio_##name##_module();
  CIO_LIB_MAP(XX)
#undef XX

#endif
