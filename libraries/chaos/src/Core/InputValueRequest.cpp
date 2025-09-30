#include "chaos/ChaosPCH.h"

#include "chaos/ChaosInternals.h"

namespace chaos
{
	InputValueRequest<Key, bool> RequestInputValue(Key const& in_key, bool& in_result)
	{
		return { in_key, in_result };
	}

	InputValueRequest<GamepadAxis, float> RequestInputValue(GamepadAxis const& in_axis, float& in_result)
	{
		return { in_axis, in_result };
	}

	InputValueRequest<GamepadStick, glm::vec2> RequestInputValue(GamepadStick const& in_stick, glm::vec2& in_result)
	{
		return { in_stick, in_result };
	}

}; // namespace chaos
