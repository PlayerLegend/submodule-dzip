#include <stddef.h>
#include <stdbool.h>
#include <stdint.h>
#include <assert.h>
#define FLAT_INCLUDES
#include "../../array/range.h"
#include "../../window/def.h"
#include "../../window/alloc.h"
#include "../../convert/def.h"
#include "../common/common.h"
#include "../inflate/inflate.h"
#include "../deflate/deflate.h"
#include "dzip.h"

#include <assert.h>
#include "../../log/log.h"

bool dzip_inflate_read (bool * error, window_unsigned_char * output, convert_interface * interface, size_t limit)
{
    dzip_convert_interface * read = (void*) interface;
    
    if (!convert_fill_limit (error, &read->read_buffer, read->next, sizeof (dzip_header)))
    {
	return false;
    }

    if (!dzip_verify_header ((dzip_header*) read->read_buffer.region.begin))
    {
	*error = true;
	return false;
    }

    if (!convert_fill_limit(error, &read->read_buffer, read->next, ((dzip_header*) read->read_buffer.region.begin)->chunk_size))
    {
	*error = true;
	return false;
    }

    if (!dzip_inflate(output, ((dzip_chunk*)read->read_buffer.region.begin)))
    {
	*error = true;
	return false;
    }

    window_release(read->read_buffer, ((dzip_header*) read->read_buffer.region.begin)->chunk_size);
    
    return true;
}

bool dzip_deflate_write (bool * error, window_unsigned_char * input, convert_interface * interface, size_t limit)
{
    dzip_convert_interface * write = (void*) interface;
    
    dzip_deflate(&write->write_buffer, write->deflate_state, &input->region.const_cast);

    window_rewrite (*input);
    
    return convert_drain (error, &write->write_buffer, write->next);
}

void dzip_convert_clear (convert_interface * interface)
{
    dzip_convert_interface * full = (dzip_convert_interface*) interface;
    
    window_clear (full->read_buffer);
    window_clear (full->write_buffer);

    if (full->deflate_state)
    {
	dzip_deflate_state_free(full->deflate_state);
    }
}

dzip_convert_interface dzip_convert_interface_init (convert_interface * next)
{
    dzip_convert_interface retval = (dzip_convert_interface)
	{
	    .next = next,
	    .interface.read = dzip_inflate_read,
	    .interface.write = dzip_deflate_write,
	    .interface.clear = dzip_convert_clear,
	    .deflate_state = dzip_deflate_state_new(),
	};

    return retval;
}

dzip_convert_interface dzip_convert_interface_init_readonly (convert_interface * next)
{
    dzip_convert_interface retval = (dzip_convert_interface)
	{
	    .next = next,
	    .interface.read = dzip_inflate_read,
	    .interface.clear = dzip_convert_clear,
	};

    return retval;
}
