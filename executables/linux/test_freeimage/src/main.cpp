#include <stdlib.h>
#include <stdio.h>
#include <iostream>

#include <FreeImage.h>

int main(int argc, char ** argv)
{
	FreeImage_Initialise(true);
	
	FreeImage_DeInitialise();

	return 0;
}
 