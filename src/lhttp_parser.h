#ifndef LHTTP_PARSER_H
#define LHTTP_PARSER_H

#include "candor.h"
#include "http_parser.h"

using namespace candor;

namespace candorIO {

  class HttpParser : public CWrapper {
    http_parser parser;
    Handle<Object> callbacks;
   public:

    int Emit(const char* name);
    int Emit(const char* name, const char *at, size_t length);
    Value* Init(uint32_t argc, Arguments& argv);
    Value* Execute(uint32_t argc, Arguments& argv);
    Value* Finish(uint32_t argc, Arguments& argv);
    Value* Pause(uint32_t argc, Arguments& argv);
  };
}


Value* http_parser_module(uint32_t argc, Arguments& argv);

#endif
