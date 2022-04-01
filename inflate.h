#ifndef FLAT_INCLUDES
#include "../convert/source.h"
#endif

/**
   @file dzip/inflate.h
   Provides functions for inflating chunks from a dzip stream
*/

status dzip_inflate_chunk (window_unsigned_char * output, convert_source * source);
/**<
   @brief Inflates a single chunk from the given source stream and appends the resulting bytes to output.
   @param error Set to true if an error occurs
   @param output The output window
   @param source The dzip source
   @return true if a chunk was successfully read, false otherwise
*/

status dzip_inflate_all (window_unsigned_char * output, convert_source * source);
/**<
   @brief Inflates all chunks from the given source stream and appends the resulting bytes to output
   @param error Set to true if an error occurs
   @param output The output window
   @param source The dzip source
   @return true if all chunks were successfully read, false otherwise
*/
