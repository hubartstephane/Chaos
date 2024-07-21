#include "teststaticlib/functions.h"

namespace static_lib
{
	int const * GetSHUIntPtr()
	{
		static int value = 0;
		return &value;
	}
}