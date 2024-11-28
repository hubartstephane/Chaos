#include "chaos/ChaosPCH.h"
#include "chaos/ChaosInternals.h"

namespace chaos
{
	namespace details
	{
		float log3(float src)
		{
			return (std::log(src) / std::log(3.0f));
		};

		float pow3(float src)
		{
			return std::pow(3.0f, src);
		};

		int log3i(int src)
		{
			return int(log3(float(src)));
		};

		int pow3i(int src)
		{
			return int(pow3(float(src)));
		};
	};

}; // namespace chaos
