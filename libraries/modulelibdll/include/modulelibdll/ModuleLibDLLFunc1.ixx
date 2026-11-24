#include "modulelibdll/Config.h"

export module modulelibdll:modulelibdll_Func1;

export class MyClass2;

export 
{
	class MODULELIBDLL_API MyClass1
	{
	public:

		MyClass1();

		void f();

	public:

		MyClass2 * other = nullptr;
	};

	MODULELIBDLL_API void ModuleLibDLLFunc1(int i);
}