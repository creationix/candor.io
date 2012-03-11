#ifndef LUV_H
#define LUV_H

#include "candor.h"

using namespace candor;

// internal flags
enum isTYPE {
  isHANDLE  = 0x01,
  isSTREAM  = 0x02,
  isTCP     = 0x04,
  isUDP     = 0x08,
  isPIPE    = 0x10,
  isTTY     = 0x20,
  isTIMER   = 0x40,
  isPROCESS = 0x80
};

enum kTYPE {
  kHANDLE   = isHANDLE,
  kSTREAM   = isHANDLE | isSTREAM,
  kTCP      = isHANDLE | isSTREAM | isTCP,
  kUDP      = isHANDLE | isUDP,
  kPIPE     = isHANDLE | isSTREAM | isPIPE,
  kTTY      = isHANDLE | isSTREAM | isTTY,
  kTIMER    = isHANDLE | isTIMER,
  kPROCESS  = isHANDLE | isPROCESS
};

void luv_init(Object* global);

#endif