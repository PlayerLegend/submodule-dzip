#include <assert.h>
#include <stddef.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#define FLAT_INCLUDES
#include "../range/def.h"
#include "../window/def.h"
#include "../window/alloc.h"
#include "../window/printf.h"
#include "../vluint/vluint.h"
#include "deflate.h"
#include "common.h"
#include "../log/log.h"

inline static dzip_deflate_offset * get_state_index (dzip_deflate_state * state, size_t i)
{
    return *(state) + i % (sizeof (dzip_deflate_state) / sizeof (dzip_deflate_offset));
}

typedef struct {
    dzip_deflate_offset offset;
    dzip_deflate_offset size;
}
    deflate_match;

inline static bool find_match (deflate_match * match, dzip_deflate_state * state, const unsigned char * input_start, range_const_unsigned_char * input)
{
    assert ((size_t)range_count(*input) >= sizeof(dzip_deflate_index));
    
    dzip_deflate_offset * index_p = get_state_index (state, *(dzip_deflate_index*) input->begin);
    match->offset = *index_p;

    size_t input_size = range_count(*input);

    if (input_start + match->offset < input->begin)
    {
	for (match->size = 0; match->size < input_size; match->size++)
	{
	    if (input_start[match->offset + match->size] != input->begin[match->size])
	    {
		break;
	    }
	}
    }
    else
    {
	match->size = 0;
    }

    *index_p = input->begin - input_start;

    return match->size > 4;
}

inline static void write_match (window_unsigned_char * output, const deflate_match * match)
{
    size_t size = match->size;

    assert (size);

    vluint_write(output, 1 | (size << 1));
    vluint_write(output, match->offset);
}

inline static void write_literal (window_unsigned_char * output, const unsigned char ** literal_begin, range_const_unsigned_char * input)
{
    size_t size = input->begin - *literal_begin;

    if (!size)
    {
	return;
    }

    vluint_write (output, 0 | (size << 1));
    
    window_append_bytes(output, *literal_begin, size);
    *literal_begin = input->begin;
}

void dzip_deflate_chunk (window_unsigned_char * output, dzip_deflate_state * state, range_const_unsigned_char * input)
{
    window_printf_append (&output->signed_cast, "%s", DZIP_MAGIC_STRING);
    vluint_write (output, DZIP_VERSION);
    vluint_write (output, range_count(*input));
    
    const unsigned char * input_start = input->begin;

    const unsigned char * literal_begin = input->begin;

    deflate_match match;

    while (range_count(*input) > (long long int)sizeof(dzip_deflate_index))
    {
	if (find_match (&match, state, input_start, input))
	{
	    write_literal (output, &literal_begin, input);
	    write_match (output, &match);
	    input->begin += match.size;
	    literal_begin = input->begin;
	}
	else
	{
	    input->begin++;
	}
    }

    input->begin = input->end;

    write_literal (output, &literal_begin, input);
}

void dzip_deflate_mem (window_unsigned_char * output, dzip_deflate_state * state, range_const_unsigned_char * input)
{
    size_t chunk_size = 1e6;
    range_const_unsigned_char chunk_input = { .begin = input->begin };

    while (chunk_input.end != input->end)
    {
	chunk_input.end = chunk_input.begin + chunk_size;
	
	if (chunk_input.end > input->end)
	{
	    chunk_input.end = input->end;
	}

	dzip_deflate_chunk (output, state, &chunk_input);
    }

    input->begin = input->end;
}
