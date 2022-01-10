#ifndef FLAT_INCLUDES
#include <stdint.h>
#define FLAT_INCLUDES
#endif

/**
   @file dzip/common.h
   Provides definitions used by both the dzip inflate and deflate functions. These are currently only used internally.
*/

#define DZIP_MAGIC_STRING "deezwhat" /// The dzip magic number
#define DZIP_MAGIC_CAST(string) ( *(uint64_t*) (string) ) /// Casts 8 chars into an integer, useful for comparing to dzip's magic number
#define DZIP_MAGIC_INT DZIP_MAGIC_CAST(DZIP_MAGIC_STRING) /// The dzip magic number cast to an int
#define DZIP_VERSION 0 /// The dzip version to check for in incoming chunks
