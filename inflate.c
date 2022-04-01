#include <stdint.h>
#include "../vluint/source.h"
#include "inflate.h"
#include "../log/log.h"
#include "common.h"
#include <assert.h>
#include "../window/alloc.h"

#define VLUINT_SOURCE_LIMIT 16

status dzip_inflate_chunk (window_unsigned_char * output, convert_source * source)
{
    size_t chunk_start = range_count(output->region);

    range_const_unsigned_char * input = &source->contents->region.const_cast;
    
    switch (convert_fill_minimum (source, sizeof(DZIP_MAGIC_INT)))
    {
    default:
    case STATUS_ERROR:
	log_fatal ("dzip chunk is missing magic number");

    case STATUS_END:
	return STATUS_END;

    case STATUS_UPDATE:
	break;
    }

    if (DZIP_MAGIC_CAST(input->begin) != DZIP_MAGIC_INT)
    {
	log_fatal ("dzip chunk magic number is incorrect");
    }

    input->begin += sizeof(DZIP_MAGIC_INT);

    vluint_result dzip_version;

    if (!vluint_read_source (&dzip_version, source, VLUINT_SOURCE_LIMIT))
    {
	log_fatal ("could not read dzip chunk version");
    }

    if (dzip_version != DZIP_VERSION)
    {
	log_fatal ("Bad dzip version in chunk");
    }

    vluint_result output_size;
    if (!vluint_read_source (&output_size, source, VLUINT_SOURCE_LIMIT))
    {
	log_fatal ("could not read dzip chunk output size");
    }

    window_alloc (*output, range_count(output->region) + output_size);

    bool is_match;
    
    vluint_result size;
    vluint_result match_offset;
    const unsigned char * src;
    unsigned char * dest;

    size_t i;

read_arg1:
    
    if (range_count (output->region) - chunk_start == output_size)
    {
	return output_size ? STATUS_UPDATE : STATUS_END;
    }

    if (!vluint_read_source (&size, source, VLUINT_SOURCE_LIMIT))
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
    
    if (!vluint_read_source (&match_offset, source, VLUINT_SOURCE_LIMIT))
    {
	log_fatal ("Failed to read dzip match offset");
    }
    
    src = output->region.end - size - match_offset;

    if (src < output->region.begin || src >= output->region.end)
    {
	log_fatal ("Match offset out of bounds %zu/%zu", output->region.end - src, range_count(output->region));
    }

    goto copy;

literal:
    
    if (STATUS_ERROR == convert_fill_minimum(source, size))
    {
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
    return STATUS_ERROR;
}

status dzip_inflate_all (window_unsigned_char * output, convert_source * source)
{
    status status = STATUS_UPDATE;
    
    while (STATUS_UPDATE == (status = dzip_inflate_chunk (output, source)))
    {
    }
    
    return status;
}
