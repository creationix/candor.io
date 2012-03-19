#include <stdio.h>
#include <stdlib.h>

#include "candor.h"
#include "uv.h"
#include "main.h"
#include "cio.h"
#include "luv.h"

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

  // Inject cio module into global scope
  cio_init(*global);

  // Inject uv module into global scope
  luv_init(*global);

  code->SetContext(*global);

  code->Call(0, NULL);

  // Start the libuv event loop
  uv_run(uv_default_loop());

  return 0;
}