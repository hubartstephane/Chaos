module;

#include "chaos2/Chaos2PCH.h"

module Core:MyOtherClass;
import Core:MyClass;

MyClass MyOtherClass::g()
{
	return {};
}
