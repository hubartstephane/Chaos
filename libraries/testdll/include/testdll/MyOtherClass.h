#include "testdll/Config.h"
#include <string>

#ifdef _MSC_VER
#  pragma warning(push)
#  pragma warning(disable: 4251)
#endif

class TESTDLL_API MyOtherClass
{
public:

	void f();

	std::string name;
};

#ifdef _MSC_VER
#  pragma warning(pop)
#endif