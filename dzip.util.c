#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <getopt.h>
#include <stdlib.h>
#include <unistd.h>
#include <assert.h>
#define FLAT_INCLUDES
#include "../range/def.h"
#include "../window/def.h"
#include "../window/alloc.h"
#include "../convert/source.h"
#include "../convert/fd/source.h"
#include "../convert/sink.h"
#include "../convert/fd/sink.h"
#include "../convert/join.h"
#include "deflate.h"
#include "inflate.h"
#include "source.h"
#include "sink.h"
#include "../log/log.h"

int main(int argc, char * argv[])
{
    bool decompress_flag = false;

    struct option long_options[] = 
	{
	    { "decompress", no_argument, NULL, 'd' },
	};

    int c;
    int option_index;

    while (0 <= (c = getopt_long (argc, argv, "d", long_options, &option_index)))
    {
	switch(c)
	{
	case 'd':
	    decompress_flag = true;
	}
    }

    if (optind != argc)
    {
	log_fatal ("Invalid arguments given");
    }

    if (decompress_flag)
    {
	window_unsigned_char fd_contents = {0};
	fd_source fd_source = fd_source_init(STDIN_FILENO, &fd_contents);
	{
	    window_unsigned_char inflate_contents = {0};
	    dzip_inflate_source inflate_source = dzip_inflate_source_init(&fd_source.source, &inflate_contents);
	    {
		fd_sink fd_sink = fd_sink_init(STDOUT_FILENO, &inflate_contents.region.const_cast);
		if (!convert_join (&fd_sink.sink, &inflate_source.source))
		{
		    log_fatal ("Failed to join input to output");
		}
	    }
	    window_clear (inflate_contents);
	}

	window_clear (fd_contents);
    }
    else
    {
	window_unsigned_char fd_contents = {0};
	window_alloc (fd_contents, 1e6);
	fd_source fd_source = fd_source_init(STDIN_FILENO, &fd_contents);
	{
	    fd_sink fd_sink = fd_sink_init(STDOUT_FILENO, NULL);
	    convert_sink * sink = dzip_deflate_sink_new(&fd_sink.sink, &fd_contents.region.const_cast);
	    
	    if (!convert_join (sink, &fd_source.source))
	    {
		log_fatal ("Failed to join input to output");
	    }
	    convert_sink_free (sink);
	}
	window_clear (fd_contents);
    }

    return 0;

fail:
    exit(1);
}
