#include <stdlib.h>
#include <stdio.h>
#include <iostream>

#include <zlib.h>

int main(int argc, char ** argv)
{
	z_stream strm;
	strm.zalloc = Z_NULL;
	strm.zfree = Z_NULL;
	strm.opaque = Z_NULL;
	strm.avail_in = 0;
	strm.next_in = Z_NULL;	
	
	if (deflateInit(&strm, Z_DEFAULT_COMPRESSION) == Z_OK)
	{
		
		deflateEnd(&strm);
	}


	std::cout << "test zlib" << std::endl;
	return 0;
}
 