#include "chaos/ChaosPCH.h"
#include "chaos/ChaosInternals.h"

namespace chaos
{
	namespace details
	{
		float log3(float value)
		{
			return (std::log(value) / std::log(3.0f));
		};

		float pow3(float level)
		{
			return std::pow(3.0f, level);
		};
	};

}; // namespace chaos
