UVDIR=deps/libuv
CANDIR=deps/candor


# verbose build
export Q=
MAKEFLAGS+=-e

DEPS=${CANDIR}/candor.a  \
     ${UVDIR}/uv.a

LIBS=build/main.o       \
     build/cio.o        \
     build/cio_string.o \
     build/luv.o        \
     build/luv_misc.o   \
     build/luv_tcp.o    \
     build/luv_timer.o

all: build/canio

${CANDIR}/candor.a: ${CANDIR}/Makefile
	$(MAKE) -C ${CANDIR} candor.a

${UVDIR}/uv.a: ${UVDIR}/Makefile
	$(MAKE) -C ${UVDIR} uv.a

build:
	mkdir -p build

build/%.o: src/%.cc build src/%.h ${DEPS}
	g++ -g -Wall -Werror -c $< -o $@ -I${UVDIR}/include -I${CANDIR}/include -D_LARGEFILE_SOURCE -D_FILE_OFFSET_BITS=64

build/canio: ${DEPS} ${LIBS}
	g++ -g -o build/canio ${LIBS} ${DEPS} -pthread -lrt

clean:
	rm -rf build

distclean: clean
	${MAKE} -C ${CANDIR} clean
	${MAKE} -C ${UVDIR} distclean

.PHONY: clean distclean all

