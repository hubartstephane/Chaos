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

	// XXX : some sticks are not abled to physicaly returns 1.0 when they are fully triggered (depend on the device)
	//       that's why i use some min/max values (initialized with a coherent value)
	//       if the stick goes further than theses values, we update them.
	//       that help us to have a good evaluation of the stick range over time.

	void Input1DState::SetValue(float in_value, float dead_zone, InputDeviceType device_type)
	{
		// update min and max values
		min_raw_value = std::min(min_raw_value, in_value);
		max_raw_value = std::max(max_raw_value, in_value);

		// apply dead zone, clamp and normalize
		if (device_type == InputDeviceType::GAMEPAD)
		{
			if (dead_zone > 0.0f)
			{
				if (in_value <= dead_zone && in_value >= -dead_zone)
				{
					in_value = 0.0f;
				}
				else if (in_value > 0.0f) // normalize input
				{
					in_value = (max_raw_value - dead_zone > 0.0f) ?
						(in_value - dead_zone) / (max_raw_value - dead_zone) :
						0.0f;
				}
				else if (in_value < 0.0f) // normalize input
				{
					in_value = (min_raw_value + dead_zone < 0.0f) ?
						-(in_value + dead_zone) / (min_raw_value + dead_zone) :
						0.0f;
				}
			}
			else
			{
				in_value = std::clamp(in_value, -1.0f, +1.0f); // clamp the raw value to -1 .. +1
			}
		}
		DoSetValue(in_value);
	}

	void Input2DState::SetValue(glm::vec2 in_value, float dead_zone, InputDeviceType device_type)
	{
		float value_length = glm::length(in_value);

		// update max length
		max_raw_length = std::max(max_raw_length, value_length);

		// apply dead zone and clamp
		if (device_type == InputDeviceType::GAMEPAD)
		{
			if (dead_zone > 0.0f)
			{
				if (value_length <= dead_zone)
				{
					in_value = { 0.0f, 0.0f };
				}
				else if (max_raw_length > dead_zone)
				{
					in_value = glm::normalize(in_value) * ((value_length - dead_zone) / (max_raw_length - dead_zone));
				}
			}
			else if (value_length > 1.0f)
			{
				in_value /= value_length;
			}
		}

		DoSetValue(in_value);
	}

}; // namespace chaos
