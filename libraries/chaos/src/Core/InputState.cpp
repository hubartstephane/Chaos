#include "chaos/ChaosPCH.h"

#include "chaos/ChaosInternals.h"

namespace chaos
{
	void KeyState::SetValue(bool in_value)
	{
		DoSetValue(in_value);
	}

	bool KeyState::IsDown() const
	{
		return value;
	}

	bool KeyState::IsUp() const
	{
		return !value;
	}

	bool KeyState::IsJustPressed() const
	{
		return 
			(IsDown()) && 
			(GetSameValueTimer() == 0.0f);
	}

	bool KeyState::IsJustReleased() const
	{
		return 
			(IsUp()) && 
			(GetSameValueTimer() == 0.0f);
	}

	KeyStatus KeyState::GetStatus() const
	{
		float same_value_time = GetSameValueTimer();

		if (value)
		{
			if (same_value_time == 0.0f && update_time >= 0.0f)
				return KeyStatus::BECOME_PRESSED;
			else
				return KeyStatus::STAY_PRESSED;
		}
		else
		{
			if (same_value_time == 0.0f && update_time >= 0.0f)
				return KeyStatus::BECOME_RELEASED;
			else
				return KeyStatus::STAY_RELEASED;
		}
	}

	void Input1DState::SetValue(float in_value)
	{
		DoSetValue(in_value);
	}

	void Input2DState::SetValue(glm::vec2 in_value)
	{
		DoSetValue(in_value);
	}

}; // namespace chaos
