#include <stddef.h>
#include <stdbool.h>
#include <stdint.h>
#include <assert.h>
#include <stdlib.h>
#define FLAT_INCLUDES
#include "../range/def.h"
#include "../window/def.h"
#include "../window/alloc.h"
#include "../convert/sink.h"
#include "sink.h"
#include "deflate.h"
#include "../log/log.h"

typedef struct {
    convert_sink sink;
    convert_sink * next;
    dzip_deflate_state state;
    window_unsigned_char next_contents;
}
    dzip_deflate_sink;

static bool update_callback(bool * error, convert_sink * target)
{
    dzip_deflate_sink * deflate_sink = (dzip_deflate_sink*) target;
    dzip_deflate_mem(&deflate_sink->next_contents, &deflate_sink->state, target->contents);
    deflate_sink->next->contents = &deflate_sink->next_contents.region.const_cast;

    return convert_drain(error, deflate_sink->next);
}

static void clear_callback(convert_sink * target)
{
    dzip_deflate_sink * deflate_sink = (dzip_deflate_sink*) target;
    window_clear (deflate_sink->next_contents);
}

convert_sink * dzip_deflate_sink_new(convert_sink * next, range_const_unsigned_char * contents)
{
    dzip_deflate_sink * deflate_sink = calloc (1, sizeof(dzip_deflate_sink));

    deflate_sink->sink = (convert_sink)
    {
	.update = update_callback,
	.clear = clear_callback,
	.contents = contents,
    };

    deflate_sink->next = next;

    return &deflate_sink->sink;
}
