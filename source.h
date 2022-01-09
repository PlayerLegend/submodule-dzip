#ifndef FLAT_INCLUDES
#include <stddef.h>
#include <stdbool.h>
#define FLAT_INCLUDES
#include "../range/def.h"
#include "../window/def.h"
#include "../convert/source.h"
#endif

typedef struct {
    convert_source source;
    convert_source * next;
}
    dzip_inflate_source;

dzip_inflate_source dzip_inflate_source_init(convert_source * next, window_unsigned_char * contents);
