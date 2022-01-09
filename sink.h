#ifndef FLAT_INCLUDES
#include <stddef.h>
#include <stdbool.h>
#define FLAT_INCLUDES
#include "../range/def.h"
#include "../window/def.h"
#include "../convert/sink.h"
#endif

convert_sink * dzip_deflate_sink_new(convert_sink * next, range_const_unsigned_char * contents);
