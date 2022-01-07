C_PROGRAMS += bin/dzip bin/dzip-benchmark
SH_PROGRAMS += test/dzip-verify bin/benchmark-compression-utils
RUN_TESTS += test/dzip-verify

dzip-tests: test/dzip-verify bin/dzip-benchmark
dzip-utils: bin/benchmark-compression-utils bin/dzip

bin/benchmark-compression-utils: src/dzip/test/compression-benchmarks.sh
bin/dzip-benchmark: src/window/alloc.o
bin/dzip-benchmark: src/dzip/deflate/deflate.o
bin/dzip-benchmark: src/dzip/test/dzip-benchmark.test.o
bin/dzip-benchmark: src/log/log.o
bin/dzip-benchmark: src/vluint/vluint.o
bin/dzip-benchmark: src/convert/fd.o
bin/dzip-benchmark: src/convert/def.o
bin/dzip: src/dzip/deflate/deflate.o
bin/dzip: src/dzip/inflate/inflate.o
bin/dzip: src/dzip/util/dzip.util.o
bin/dzip: src/log/log.o
bin/dzip: src/vluint/vluint.o
bin/dzip: src/convert/def.o
bin/dzip: src/convert/fd-bifurcated.o
bin/dzip: src/window/alloc.o
bin/dzip: src/dzip/convert/dzip.o
test/dzip-verify: src/dzip/test/dzip-verify.test.sh

tests: dzip-tests
utils: dzip-utils
