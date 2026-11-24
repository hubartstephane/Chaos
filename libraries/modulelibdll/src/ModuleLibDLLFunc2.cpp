module;

#include <iostream>

module modulelibdll;

void ModuleLibDLLFunc2(int i)
{
	std::cout << "ModuleLibDLLFunc2: " << i << std::endl;
	if (i < 0)
		return;
	ModuleLibDLLFunc1(i - 1);
}