#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include <assert.h>
#define FLAT_INCLUDES
#include "../../keyargs/keyargs.h"
#include "../../array/range.h"
#include "../../window/def.h"
#include "../../window/alloc.h"
#include "../common/common.h"
#include "inflate.h"
#include "../common/internal.h"
#include "../../log/log.h"
#include "../../vluint/vluint.h"
#include "../../libc/string.h"

bool dzip_inflate (window_unsigned_char * output, const dzip_chunk * chunk)
{
    if (!dzip_verify_header((void*)chunk))
    {
	log_fatal ("Failed to verify header");
    }

    assert (chunk->bytes == (const unsigned char *) chunk + sizeof(dzip_header));
    
    range_const_unsigned_char input = { .begin = chunk->bytes, .end = ((const unsigned char*) chunk) + chunk->header.chunk_size };

    if (range_count (input) <= 0)
    {
	log_fatal ("Invalid size in dzip header");
    }

    char cmd;
    
    vluint_result arg1;
    vluint_result arg2;

    dzip_size output_size;
    unsigned char copy;
    bool extend;

read_cmd:

    if (range_is_empty(input))
    {
	return true;
    }

    //log_debug ("reading cmd %zu at %zu", *input.begin, input.begin - (const unsigned char*)chunk);
    
    cmd = *input.begin & DZIP_COMMAND_MASK;
    arg1 = (*input.begin >> DZIP_META_BITS) & DZIP_ARG1_COMPACT_MASK;
    extend = *input.begin & DZIP_ARG1_EXTEND_BIT;
    
    input.begin++;

    if (extend)
    {
	goto read_extended_arg1;
    }
    else
    {
	goto cmd_switch;
    }

    assert(false);

read_extended_arg1:

    //log_debug ("extended");

    if (!vluint_read (.result = &arg2,
		      .input = &input,
		      .rest = &input))
    {
	//log_debug ("Failed at byte %zu", input.begin - (const unsigned char*)chunk - sizeof(dzip_header));
	log_fatal ("Truncated extended arg1");
    }
    
    arg1 += DZIP_ARG1_COMPACT_MAX * arg2;

    goto cmd_switch;

cmd_switch:

    //log_debug ("cmd switch %d %d", cmd, arg1);
    
    switch (cmd)
    {
    case DZIP_CMD_LITERAL: goto read_literal;
    case DZIP_CMD_MATCH: goto read_match;
    default: log_fatal ("Invalid dzip command");
    }

    assert (false);

read_literal:

    //log_debug ("literal");
    
    if (arg1 == 0)
    {
	log_fatal ("dzip literal length is zero");
    }

    if (arg1 > (dzip_size)range_count(input))
    {
	log_fatal ("dzip literal length is too long");
    }

    window_append_bytes(output, input.begin, arg1);
    
    input.begin += arg1;

    goto read_cmd;

read_match:

    //log_debug ("match");
    
    if (arg1 == 0)
    {
	log_fatal ("dzip match length is zero");
    }

    if (!vluint_read(.result = &arg2,
		     .input = &input,
		     .rest = &input))
    {
	log_fatal ("dzip match distance vluint is truncated");
    }

    if (arg2 == 0)
    {
	log_fatal ("dzip match distance is zero");
    }

    output_size = range_count(output->region);
    
    if (arg2 > output_size)
    {
	log_fatal ("dzip match distance is too large");
    }

    while (arg1 > 0)
    {
	copy = output->region.end[-arg2];
	*window_push(*output) = copy;
	arg1--;
    }

    goto read_cmd;
    
fail:
    return false;
}
