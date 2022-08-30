//#define _SILENCE_CLANG_CONCEPTS_MESSAGE
#include <iostream>

#include "testdll/MyOtherClass.h"

import MyClass;

int main(int argc, char ** argv, char ** env)
{

	MyClass().f();
	MyOtherClass().f();

	std::cout << "Hello World Again" << std::endl;

	return 0;
}