#include "chaos/ChaosPCH.h"

#include "chaos/ChaosInternals.h"

namespace chaos
{
	QueryValueInputRequest<Key, bool> QueryValue(Key in_key, bool& in_result, bool in_fail_on_inactive_input)
	{
		return { in_key, in_result, in_fail_on_inactive_input };
	}

	QueryValueInputRequest<Input1D, float> QueryValue(Input1D in_axis, float& in_result, bool in_fail_on_inactive_input)
	{
		return { in_axis, in_result, in_fail_on_inactive_input };
	}

	QueryValueInputRequest<Input2D, glm::vec2> QueryValue(Input2D in_stick, glm::vec2& in_result, bool in_fail_on_inactive_input)
	{
		return { in_stick, in_result, in_fail_on_inactive_input };
	}

}; // namespace chaos
