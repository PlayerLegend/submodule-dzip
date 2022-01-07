#ifndef FLAT_INCLUDES
#include <stddef.h>
#include <stdbool.h>
#define FLAT_INCLUDES
#include "../../array/range.h"
#include "../../window/def.h"
#include "../../convert/def.h"
#include "../deflate/deflate.h"
#endif

typedef struct {
    convert_interface interface;
    convert_interface * next;
    window_unsigned_char read_buffer;
    window_unsigned_char write_buffer;
    dzip_deflate_state * deflate_state;
}
    dzip_convert_interface;

dzip_convert_interface dzip_convert_interface_init (convert_interface * next);
dzip_convert_interface dzip_convert_interface_init_readonly (convert_interface * next);
