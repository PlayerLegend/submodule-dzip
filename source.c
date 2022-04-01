#include "source.h"
#include "inflate.h"
#include "../log/log.h"
#include <assert.h>

static status update_callback(convert_source * target)
{
    dzip_inflate_source * inflate_source = (dzip_inflate_source*) target;

    return dzip_inflate_chunk (inflate_source->source.contents, inflate_source->next);
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
