#include <stdlib.h>
#include <stdio.h>
#include <iostream>

#include <ft2build.h>
#include FT_FREETYPE_H
#include FT_GLYPH_H
#include FT_BITMAP_H

int main(int argc, char ** argv)
{
	FT_Library  library;
	FT_Init_FreeType( &library );
	
	std::cout << "test freetype2" << std::endl;
	
	return 0;
}
 