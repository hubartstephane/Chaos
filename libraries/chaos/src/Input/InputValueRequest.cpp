#include "chaos/ChaosPCH.h"

#include "chaos/ChaosInternals.h"

namespace chaos
{
	InputValueRequest<Key, bool> RequestInputValue(Key in_key, bool& in_result)
	{
		return { in_key, in_result };
	}

	InputValueRequest<Input1D, float> RequestInputValue(Input1D in_axis, float& in_result)
	{
		return { in_axis, in_result };
	}

	InputValueRequest<Input2D, glm::vec2> RequestInputValue(Input2D in_stick, glm::vec2& in_result)
	{
		return { in_stick, in_result };
	}

}; // namespace chaos
