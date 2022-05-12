#include <getopt.h>
#include <stdlib.h>
#include <unistd.h>
#include <assert.h>

#include "../convert/join.h"
#include "source.h"
#include "inflate.h"
#include "sink.h"
#include "../log/log.h"
#include "../convert/fd/source.h"
#include "../convert/fd/sink.h"
#include "../window/alloc.h"

void print_help (const char * program_name)
{
    log_stderr("usage %s [options]", program_name);
    log_stderr("");
    log_stderr("This program reads from stdin and writes from stdout. With no options, it will compress its input.");
    log_stderr("");
    log_stderr("Optional command line flags:");
    log_stderr("\t-d: decompress input");

    log_stderr("");
}

int main(int argc, char * argv[])
{
    bool decompress_flag = false;

    struct option long_options[] = 
	{
	    { "decompress", no_argument, NULL, 'd' },
	    { "help", no_argument, NULL, 'h' },
	};

    int c;
    int option_index;

    while (0 <= (c = getopt_long (argc, argv, "dh", long_options, &option_index)))
    {
	switch(c)
	{
	case 'd':
	    decompress_flag = true;
	    break;

	case 'h':
	    print_help(argv[0]);
	    return 0;
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
		fd_sink fd_sink = fd_sink_init(STDOUT_FILENO, &inflate_contents.region.alias_const);
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
	    convert_sink * sink = dzip_deflate_sink_new(&fd_sink.sink, &fd_contents.region.alias_const);
	    
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
