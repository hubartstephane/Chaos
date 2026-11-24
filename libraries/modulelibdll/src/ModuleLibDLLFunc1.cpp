module;

#include <iostream>

module modulelibdll;

void ModuleLibDLLFunc1(int i)
{
	std::cout << "ModuleLibDLLFunc1: " << i << std::endl;
	if (i < 0)
		return;
	ModuleLibDLLFunc2(i - 1);
}