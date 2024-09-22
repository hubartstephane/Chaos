#include <stdlib.h>
#include <stdio.h>
#include <iostream>

#include <irrKlang.h>

using namespace irrklang;

int main(int argc, char ** argv)
{
	ISoundEngine * engine = createIrrKlangDevice();
	
	engine->drop();
	
	std::cout << "test irrklang" << std::endl;	
	return 0;
}
 