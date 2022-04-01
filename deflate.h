#ifndef FLAT_INCLUDES
#include <stdint.h>
#include "../window/def.h"
#endif

/**
   @file dzip/deflate.h
   Provides deflate functions for dzip streams
*/

typedef uint32_t dzip_deflate_offset; ///< An integer large enough to describe an offset within a dzip chunk
typedef uint64_t dzip_deflate_index; ///< An integer used to index a dzip table
typedef dzip_deflate_offset dzip_deflate_state[16963]; ///< A dzip table used to find matches within a chunk

void dzip_deflate_mem (window_unsigned_char * output, dzip_deflate_state * state, range_const_unsigned_char * input); ///< Consumes all bytes from the given input and compresses them into one or more chunks appended to the output. state need not be initialized
void dzip_deflate_chunk (window_unsigned_char * output, dzip_deflate_state * state, range_const_unsigned_char * input); ///< Consumes all bytes from the given input and compresses them into a single chunk appended to the output
