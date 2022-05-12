#include "../deflate.h"
#include <sys/time.h>
#include "../../convert/status.h"
#include "../../convert/source.h"
#include "../../convert/sink.h"
#include "../../convert/fd/source.h"
#include "../../convert/fd/sink.h"
#include <unistd.h>
#include <assert.h>
#include "../../window/alloc.h"
#include "../../log/log.h"
#include <stdlib.h>

typedef unsigned long long usec;

struct timeval start;

void start_time()
{
    gettimeofday (&start, NULL); 
}

usec get_delta()
{
    struct timeval t;
    gettimeofday (&t, NULL);
    return 1e6 * (t.tv_sec - start.tv_sec) + (t.tv_usec - start.tv_usec);
}

void sliced()
{
    window_unsigned_char source_contents = {0};
    fd_source fd_source = fd_source_init(STDIN_FILENO, &source_contents);

    window_unsigned_char sink_contents = {0};
    fd_sink fd_sink = fd_sink_init(STDOUT_FILENO, &sink_contents.region.alias_const);

    dzip_deflate_state state = {0};

    size_t total_input_size = 0;
    size_t total_output_size = 0;
    
    assert (state);

    window_alloc(source_contents, 1e6);

    start_time();

    status status = STATUS_UPDATE;

    while ( (status = convert_fill_alloc(&fd_source.source)) != STATUS_ERROR && !range_is_empty(source_contents.region) )
    {
	//log_debug ("read %zu", range_count(source_contents.region));
		
	total_input_size += range_count(source_contents.region);
	
	dzip_deflate_mem(&sink_contents, &state, &source_contents.region.alias_const);
        
	total_output_size += range_count (sink_contents.region);

	if (STATUS_UPDATE != convert_drain (&fd_sink.sink))
	{
	    log_fatal ("Failed to write to output");
	}

	assert (range_is_empty(source_contents.region));
	assert (range_is_empty(sink_contents.region));
    }

    assert (status != STATUS_ERROR);
    
    //dzip_deflate(.state = state, .output = &output);

    //usec compress_time = get_delta();
    //double compress_seconds = (double)compress_time / (double) 1e6;
    
    //log_debug ("size ratio result/input = %d / %d =  %lf\n", (int)total_output_size, (int)total_input_size, (double) total_output_size / (double) total_input_size);
    //log_debug ("internal time: %lf seconds\n", compress_seconds);
    //log_debug ("compress megabytes per second: %lf\n", (double) total_input_size / (compress_seconds * 1e6));

    window_clear (source_contents);
    window_clear (sink_contents);

    return;

fail:
    exit(1);
}

int main()
{
    sliced();
    //full();
    
//#ifndef NDEBUG
    //dzip_print_stats();
//#endif
    return 0;
}
