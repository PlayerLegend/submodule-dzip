#include <stddef.h>
#include <stdbool.h>
#include <assert.h>
#define FLAT_INCLUDES
#include "../range/def.h"
#include "../window/def.h"
#include "../convert/source.h"
#include "source.h"
#include "inflate.h"
#include "../log/log.h"

bool update_callback(bool * error, convert_source * target)
{
    dzip_inflate_source * inflate_source = (dzip_inflate_source*) target;

    if (!dzip_inflate_chunk (error, inflate_source->source.contents, inflate_source->next))
    {
	return false;
    }

    assert (!*error);
    
    return true;
}

void clear_callback(convert_source * target)
{
    
}

dzip_inflate_source dzip_inflate_source_init(convert_source * next, window_unsigned_char * contents)
{
    return (dzip_inflate_source)
    {
	.source = { .update = update_callback, .clear = clear_callback, .contents = contents },
	.next = next,
    };
}
