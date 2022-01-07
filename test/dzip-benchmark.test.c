#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <sys/time.h>
#include <string.h>
#define FLAT_INCLUDES
#include "../../array/range.h"
#include "../../window/def.h"
#include "../../window/alloc.h"
#include "../../convert/def.h"
#include "../../convert/fd.h"
#include "../deflate/deflate.h"
#include "../common/common.h"
#include "../../log/log.h"

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
    window_unsigned_char output = {0};
    window_unsigned_char input = {0};

    dzip_deflate_state * state = dzip_deflate_state_new();

    dzip_size total_input_size = 0;
    dzip_size total_output_size = 0;
    
    assert (state);

    start_time();

    bool error = false;

    window_alloc (input, 1e6);

    convert_interface_fd read = convert_interface_fd_init(STDIN_FILENO);

    while (convert_fill(&error, &input, &read.interface))
    {
//	log_debug ("read %zu", range_count(input.region));
	dzip_deflate(&output, state, &input.region.const_cast);
	
	total_input_size += range_count(input.region);
	total_output_size += range_count (output.region);
	
	window_rewrite (output);
	window_rewrite (input);
    }

    assert (!error);
    
    //dzip_deflate(.state = state, .output = &output);

    usec compress_time = get_delta();
    double compress_seconds = (double)compress_time / (double) 1e6;
    
    printf("size ratio result/input = %d / %d =  %lf\n", (int)total_output_size, (int)total_input_size, (double) total_output_size / (double) total_input_size);
    printf("internal time: %lf seconds\n", compress_seconds);
    printf("compress megabytes per second: %lf\n", (double) total_input_size / (compress_seconds * 1e6));

    free (output.alloc.begin);
    free (input.alloc.begin);
}

int main()
{
    sliced();
    //full();
    
//#ifndef NDEBUG
    dzip_print_stats();
//#endif
    return 0;
}
