module;

#include "chaos2/Chaos2PCH.h"

module Core:MyClass;
//import Core:MyOtherClass;

void MyClass::f()
{
	int i = 0;
	++i;
}

#if 1
MyOtherClass MyClass::g()
{
	return {};
}
#endif