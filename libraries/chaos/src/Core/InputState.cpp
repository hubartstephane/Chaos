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

	void Input1DState::SetValue(float in_value, float dead_zone, float max_zone)
	{
		// apply dead zone, clamp and normalize
		if (dead_zone >= 0.0f && max_zone > dead_zone)
		{
			if (in_value <= dead_zone && in_value >= -dead_zone)
			{
				in_value = 0.0f;
			}
			else if (in_value > 0.0f) // normalize input
			{
				in_value = (in_value - dead_zone) / (max_zone - dead_zone);
				if (in_value > 1.0f)
					in_value = 1.0f;
			}
			else if (in_value < 0.0f) // normalize input
			{
				in_value = (in_value + dead_zone) / (max_zone - dead_zone);
				if (in_value < -1.0f)
					in_value = -1.0f;
			}
		}
		DoSetValue(in_value);
	}

	void Input2DState::SetValue(glm::vec2 in_value, float dead_zone, float max_zone)
	{
		// apply dead zone, clamp and normalize
		if (dead_zone >= 0.0f && max_zone > dead_zone)
		{
			float value_length = glm::length(in_value);

			if (value_length <= dead_zone)
			{
				in_value = { 0.0f, 0.0f };
			}
			else if (value_length >= max_zone)
			{
				in_value /= value_length; 
			}
			else
			{
				in_value = 
					(in_value / value_length) * 
					(value_length - dead_zone) / (max_zone - dead_zone);
			}

		}
		DoSetValue(in_value);
	}

}; // namespace chaos
