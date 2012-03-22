UVDIR=deps/libuv
CANDIR=deps/candor
HTTPDIR=deps/http-parser

LDFLAGS+=-pthread

OS_NAME=$(shell uname -s)
ifeq (${OS_NAME},Linux)
LDFLAGS+= -lrt
endif

# verbose build
export Q=
MAKEFLAGS+=-e

DEPS=${CANDIR}/candor.a  \
     ${HTTPDIR}/http_parser.o \
     ${UVDIR}/uv.a

LIBS=build/main.o       \
     build/lhttp_parser.o \
     build/cio.o        \
     build/cio_libs.o   \
     build/cio_string.o \
     build/luv.o        \
     build/luv_base.o   \
     build/luv_handle.o \
     build/luv_stream.o \
     build/luv_tcp.o    \
     build/luv_timer.o  \
     build/net.o

all: build/canio

${CANDIR}/candor.a: ${CANDIR}/Makefile
	$(MAKE) -C ${CANDIR} candor.a

${UVDIR}/uv.a: ${UVDIR}/Makefile
	$(MAKE) -C ${UVDIR} uv.a

${HTTPDIR}/http_parser.o: ${HTTPDIR}/Makefile
	$(MAKE) -C ${HTTPDIR} http_parser.o

build:
	mkdir -p build

build/%.o: lib/%.can build
	objcopy --input binary --output elf64-x86-64 --binary-architecture i386 $< $@

build/%.o: src/%.cc build src/%.h ${DEPS}
	g++ -g -Wall -Werror -c $< -o $@ -I${HTTPDIR} -I${UVDIR}/include -I${CANDIR}/include -D_LARGEFILE_SOURCE -D_FILE_OFFSET_BITS=64

build/canio: ${DEPS} ${LIBS}
	g++ -g -o build/canio ${LIBS} ${DEPS} ${LDFLAGS}

clean:
	rm -rf build

distclean: clean
	${MAKE} -C ${CANDIR} clean
	${MAKE} -C ${UVDIR} distclean
	${MAKE} -C ${HTTPDIR} clean

.PHONY: clean distclean all

