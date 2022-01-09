#ifndef FLAT_INCLUDES
#include <stddef.h>
#include <stdbool.h>
#include <stdint.h>
#define FLAT_INCLUDES
#include "../range/def.h"
#include "../window/def.h"
#include "../convert/source.h"
#endif

bool dzip_inflate_chunk (bool * error, window_unsigned_char * output, convert_source * source);
bool dzip_inflate_all (bool * error, window_unsigned_char * output, convert_source * source);
