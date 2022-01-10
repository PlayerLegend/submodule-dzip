#ifndef FLAT_INCLUDES
#include <stddef.h>
#include <stdbool.h>
#define FLAT_INCLUDES
#include "../range/def.h"
#include "../window/def.h"
#include "../convert/sink.h"
#endif

/**<
   @file dzip/sink.h
   Provides a writeable deflate interface that is compatible with the sink described in convert/sink.h
*/

convert_sink * dzip_deflate_sink_new(convert_sink * next, range_const_unsigned_char * contents);
/**<
   @brief Creates a new deflate sink
   @param next The sink to which deflated data will be written
   @param contents The contents that this sink will deflate
   @return The sink handle
*/
