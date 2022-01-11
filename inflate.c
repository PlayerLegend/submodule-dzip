#include <stddef.h>
#include <stdbool.h>
#include <stdint.h>
#include <assert.h>
#define FLAT_INCLUDES
#include "../range/def.h"
#include "../window/def.h"
#include "../convert/source.h"
#include "../window/alloc.h"
#include "inflate.h"
#include "common.h"
#include "../log/log.h"
#include "../vluint/vluint.h"
#include "../vluint/source.h"

#define VLUINT_SOURCE_LIMIT 16

bool dzip_inflate_chunk (bool * error, window_unsigned_char * output, convert_source * source)
{
    size_t chunk_start = range_count(output->region);

    range_const_unsigned_char * input = &source->contents->region.const_cast;

    if (!convert_fill_minimum (error, source, sizeof(DZIP_MAGIC_INT)))
    {
	if (*error)
	{
	    log_fatal ("dzip chunk is missing magic number");
	}
	else
	{
	    return false;
	}
    }

    if (DZIP_MAGIC_CAST(input->begin) != DZIP_MAGIC_INT)
    {
	log_fatal ("dzip chunk magic number is incorrect");
    }

    input->begin += sizeof(DZIP_MAGIC_INT);

    unsigned long long dzip_version = vluint_read_source(error, source, VLUINT_SOURCE_LIMIT);

    if (*error)
    {
	log_fatal ("could not read dzip chunk version");
    }

    if (dzip_version != DZIP_VERSION)
    {
	log_fatal ("Bad dzip version in chunk");
    }

    unsigned long long output_size = vluint_read_source(error, source, VLUINT_SOURCE_LIMIT);

    if (*error)
    {
	log_fatal ("could not read dzip chunk output size");
    }

    window_alloc (*output, range_count(output->region) + output_size);

    bool is_match;
    
    unsigned long long size;
    const unsigned char * src;
    unsigned char * dest;

    size_t i;

read_arg1:
    if (range_count (output->region) - chunk_start == output_size)
    {
	return true;
    }
    
    size = vluint_read_source (error, source, VLUINT_SOURCE_LIMIT);

    if (*error)
    {
	log_fatal ("Failed to read dzip chunk arg1");
    }

    is_match = size & 1;

    if (size & 2)
    {
	log_fatal ("Extend bit is set, but this dzip version does not understand that");
    }

    size >>= 2;

    window_grow_bytes(output, size);
    dest = output->region.end - size;
    
    if (is_match)
    {
	goto match;
    }
    else
    {
	goto literal;
    }

match:
    src = output->region.end - size - vluint_read_source (error, source, VLUINT_SOURCE_LIMIT);

    if (*error)
    {
	log_fatal ("Failed to read dzip match offset");
    }

    if (src < output->region.begin || src >= output->region.end)
    {
	log_fatal ("Match offset out of bounds %zu/%zu", output->region.end - src, range_count(output->region));
    }

    goto copy;

literal:
    if (!convert_fill_minimum(error, source, size))
    {
	assert (!*error);
	log_fatal ("Failed to read literal contents");
    }

    if ((long long int)size > range_count(*input))
    {
	log_fatal ("Literal size out of bounds");
    }
    
    src = input->begin;
    input->begin += size;
    
    goto copy;
    
copy:
    
    for (i = 0; i < size; i++)
    {
	dest[i] = src[i];
    }

    goto read_arg1;

fail:
    log_debug("inflate fail");
    *error = true;
    return false;
}

bool dzip_inflate_all (bool * error, window_unsigned_char * output, convert_source * source)
{
    while (dzip_inflate_chunk (error, output, source))
    {
    }
    
    return !*error;
}
