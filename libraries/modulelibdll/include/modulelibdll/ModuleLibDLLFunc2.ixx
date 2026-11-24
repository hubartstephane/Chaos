#include "modulelibdll/Config.h"

export module modulelibdll:modulelibdll_Func2;

export class MyClass1;

export 
{
	class MODULELIBDLL_API MyClass2
	{
	public:

		MyClass2();

		void f();

	public:

		MyClass1* other = nullptr;
	};

	MODULELIBDLL_API void ModuleLibDLLFunc2(int i);
}