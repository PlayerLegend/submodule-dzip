bin/dzip-benchmark: \
	src/dzip/test/dzip-benchmark.test.o \
	src/dzip/deflate.o \
	src/convert/source.o \
	src/convert/fd/source.o \
	src/window/printf_append.o \
	src/vluint/vluint.o \
	src/window/alloc.o \
	src/log/log.o \
	src/window/vprintf_append.o \
	src/convert/sink.o \
	src/convert/fd/sink.o \
	src/convert/join.o \

bin/dzip: \
	src/dzip/dzip.util.o \
	src/dzip/deflate.o \
	src/dzip/inflate.o \
	src/dzip/source.o \
	src/dzip/sink.o \
	src/convert/source.o \
	src/convert/sink.o \
	src/convert/fd/source.o \
	src/convert/fd/sink.o \
	src/log/log.o \
	src/vluint/vluint.o \
	src/vluint/source.o \
	src/window/alloc.o \
	src/window/printf_append.o \
	src/window/vprintf_append.o \
	src/convert/join.o \

C_PROGRAMS += bin/dzip
C_PROGRAMS += bin/dzip-benchmark
SH_PROGRAMS += test/dzip-verify
SH_PROGRAMS += bin/benchmark-compression-utils

run-tests: run-dzip-tests
run-dzip-tests:
	DEPENDS='dzip-tests dzip-utils' sh run-tests.sh test/dzip-verify

depend: dzip-depend
dzip-depend:
	cdeps src/dzip > src/dzip/depends.makefile

dzip-tests: test/dzip-verify
dzip-tests: bin/dzip-benchmark
dzip-utils: bin/benchmark-compression-utils
dzip-utils: bin/dzip

bin/benchmark-compression-utils: src/dzip/test/compression-benchmarks.sh
test/dzip-verify: src/dzip/test/dzip-verify.test.sh

tests: dzip-tests
