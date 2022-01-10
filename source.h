#ifndef FLAT_INCLUDES
#include <stddef.h>
#include <stdbool.h>
#define FLAT_INCLUDES
#include "../range/def.h"
#include "../window/def.h"
#include "../convert/source.h"
#endif

/**
   @file dzip/source.h
   Provides a readable inflate source that is compatible with the source described in convert/source.h
*/

typedef struct dzip_inflate_source /// The dzip source structure
{
    convert_source source; /// The source to read inflated bytes from
    convert_source * next; /// The source that this source will read compressed bytes from
}
    dzip_inflate_source;

dzip_inflate_source dzip_inflate_source_init(convert_source * next, window_unsigned_char * contents);
/**<
   @brief Initializes a dzip_inflate_source
   @param next A source providing compressed bytes that the new source will inflate
   @param contents A window that the new source will fill with inflated bytes
*/
