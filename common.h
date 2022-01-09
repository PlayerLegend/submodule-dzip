#ifndef FLAT_INCLUDES
#include <stdint.h>
#define FLAT_INCLUDES
#endif

#define DZIP_MAGIC_STRING "deezwhat"
#define DZIP_MAGIC_CAST(string) ( *(uint64_t*) (string) )
#define DZIP_MAGIC_INT DZIP_MAGIC_CAST(DZIP_MAGIC_STRING)
#define DZIP_VERSION 0
