#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <assert.h>
#define FLAT_INCLUDES
#include "../../array/range.h"
#include "../../window/def.h"
#include "../../window/alloc.h"
#include "../../convert/def.h"
#include "../../convert/fd-bifurcated.h"
#include "../deflate/deflate.h"
#include "../convert/dzip.h"
#include "../common/common.h"
#include "../../log/log.h"

int main(int argc, char * argv[])
{
    bool should_deflate = true;
    
    for (int i = 0; i < argc; i++)
    {
	if (0 == strcmp (argv[i], "-d"))
	{
	    should_deflate = false;
	}
    }

    convert_interface_bifurcated_fd fd_io = convert_interface_bifurcated_fd_init(STDIN_FILENO, STDOUT_FILENO);
    dzip_convert_interface dzip_io = dzip_convert_interface_init(&fd_io.interface);
    
    window_unsigned_char buffer = {0};
    window_alloc (buffer, 1e7);
    bool error = false;
    
    if (should_deflate)
    {
	while (convert_fill (&error, &buffer, &fd_io.interface))
	{
	    if (!convert_drain (&error, &buffer, &dzip_io.interface))
	    {
		log_fatal ("Could not drain deflate output");
	    }
	}

	//log_debug ("alloc size deflate %zu, %zu", range_count(buffer.alloc), range_count (deflate.result_buffer.alloc));
    }
    else
    {
	while (convert_fill (&error, &buffer, &dzip_io.interface))
	{
	    if (!convert_drain (&error, &buffer, &fd_io.interface))
	    {
		log_fatal ("Could not drain inflate output");
	    }
	}

	//log_debug ("alloc size inflate %zu, %zu", range_count(buffer.alloc), range_count (inflate.next_buffer.alloc));
    }

    convert_clear (&dzip_io.interface);
    window_clear (buffer);

    if (error)
    {
	log_fatal ("A read error occurred");
    }
    
    return 0;

fail:
    return 1;
}
