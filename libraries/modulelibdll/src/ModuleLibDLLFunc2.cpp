module;

#include <iostream>

module modulelibdll;

MyClass2::MyClass2()
{
	int i = 0;
	++i;
}

void MyClass2::f()
{
	if (other == nullptr)
		other = new MyClass1;
}
void ModuleLibDLLFunc2(int i)
{
	std::cout << "ModuleLibDLLFunc2: " << i << std::endl;
	if (i < 0)
		return;
	ModuleLibDLLFunc1(i - 1);
}