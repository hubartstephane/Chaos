module;

#include <iostream>

module modulelibdll;

MyClass1::MyClass1()
{
	int i = 0;
	++i;
}

void MyClass1::f()
{
	if (other == nullptr)
		other = new MyClass2;
}

void ModuleLibDLLFunc1(int i)
{
	std::cout << "ModuleLibDLLFunc1: " << i << std::endl;
	if (i < 0)
		return;
	ModuleLibDLLFunc2(i - 1);
}