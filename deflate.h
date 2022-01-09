#ifndef FLAT_INCLUDES
#include <stddef.h>
#include <stdbool.h>
#include <stdint.h>
#define FLAT_INCLUDES
#include "../range/def.h"
#include "../window/def.h"
#endif

typedef uint32_t dzip_deflate_offset;
typedef uint64_t dzip_deflate_index;
typedef dzip_deflate_offset dzip_deflate_state[16963];

void dzip_deflate_mem (window_unsigned_char * output, dzip_deflate_state * state, range_const_unsigned_char * input);
void dzip_deflate_chunk (window_unsigned_char * output, dzip_deflate_state * state, range_const_unsigned_char * input);
