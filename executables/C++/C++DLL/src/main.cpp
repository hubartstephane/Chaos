//#define _SILENCE_CLANG_CONCEPTS_MESSAGE
#include <iostream>

#include "testdll/MyOtherClass.h"

import MyClass;

int main(int argc, char ** argv, char ** env)
{

	MyClass m;
	m.f();
	MyOtherClass mm;
	mm.f();

	auto x = mm.name;

	std::cout << "Hello World Again" << std::endl;

	return 0;
}