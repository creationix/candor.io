// Needed for APIs used in this file
#include "candor.h"
#include "uv.h"
// Our own header
#include "main.h"
// The base system
#include "cio.h"
// Addon modules
#include "cio_string.h"
#include "luv.h"
#include "luv_tcp.h"
#include "luv_timer.h"
#include "lhttp_parser.h"

// Grab some C libraries
#include <stdio.h> // fprintf
#include <stdlib.h> // abort
#include <unistd.h> // open, lseek
#include <fcntl.h> // O_RDONLY, ...
#include <sys/types.h> // off_t
#include <string.h> // memcpy


using namespace candor;

static const char* ReadContents(const char* filename, off_t* size) {
  int fd = open(filename, O_RDONLY, S_IRUSR | S_IRGRP);
  if (fd == -1) {
    fprintf(stderr, "init: failed to open file %s\n", filename);
    abort();
  }

  off_t s = lseek(fd, 0, SEEK_END);
  if (s == -1) {
    fprintf(stderr, "init: failed to get filesize of %s\n", filename);
    abort();
  }

  char* contents = new char[s];
  if (pread(fd, contents, s, 0) != s) {
    fprintf(stderr, "init: failed to get contents of %s\n", filename);
    delete contents;
    abort();
  }

  close(fd);

  *size = s;
  return contents;
}


int main(int argc, char** argv) {

  if (argc < 2) {
    printf("Usage:\n\t%s scriptname.can\n", argv[0]);
    exit(1);
  }

  // Create a new Isolate
  Isolate isolate;

  // Load script and run
  off_t size = 0;
  const char* script = ReadContents(argv[1], &size);
  Function* code = Function::New(script, size);
  delete script;

  if (isolate.HasError()) {
    isolate.PrintError();
    exit(1);
  }


  // Create a global context
  Handle<Object> global(Object::New());
  // Inject cio module into global scope, it returns an object where we need
  // to store all module creation functions.
  Object* builtins = cio_init(*global);

  // Store other native modules for lazy-loading
  builtins->Set("string", Function::New(cio_string_module));
  builtins->Set("uv", Function::New(uv_base_module));
  builtins->Set("timer", Function::New(uv_timer_module));
  builtins->Set("tcp", Function::New(uv_tcp_module));
  builtins->Set("http_parser", Function::New(http_parser_module));

  code->SetContext(*global);
  code->Call(0, NULL);

  // Start the libuv event loop
  uv_run(uv_default_loop());

  return 0;
}

