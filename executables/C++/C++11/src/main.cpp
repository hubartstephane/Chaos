//#define _SILENCE_CLANG_CONCEPTS_MESSAGE
#include <iostream>

#include "MyOtherClass.h"

int main(int argc, char ** argv, char ** env)
{
	MyOtherClass().f();

	std::cout << "Hello World Again" << std::endl;

	return 0;
}