#include "chaos/ChaosPCH.h"

#include "chaos/ChaosInternals.h"

namespace chaos
{
	void ButtonState::SetValue(bool in_value)
	{
		double frame_time = FrameTimeManager::GetInstance()->GetCurrentFrameTime();

		if (update_time < 0.0) // very first initialization
		{
			value = in_value;
			update_time = frame_time;
		}
		else if (value != in_value)
		{
			update_time = frame_time;
			value = in_value;
		}
	}

	bool ButtonState::IsDown() const
	{
		return value;
	}

	bool ButtonState::IsUp() const
	{
		return !value;
	}

	bool ButtonState::IsJustPressed() const
	{
		return 
			(IsDown()) && 
			(GetSameValueTimer() == 0.0f);
	}

	bool ButtonState::IsJustReleased() const
	{
		return 
			(IsUp()) && 
			(GetSameValueTimer() == 0.0f);
	}

	ButtonStatus ButtonState::GetStatus() const
	{
		float same_value_time = GetSameValueTimer();

		if (value)
		{
			if (same_value_time == 0.0f)
				return ButtonStatus::BECOME_PRESSED;
			else
				return ButtonStatus::STAY_PRESSED;
		}
		else
		{
			if (same_value_time == 0.0f)
				return ButtonStatus::BECOME_RELEASED;
			else
				return ButtonStatus::STAY_RELEASED;
		}
	}

	// XXX : some sticks are not abled to physicaly returns 1.0 when they are fully triggered (depend on the device)
	//       that's why i use some min/max values (initialized with a coherent value)
	//       if the stick goes further than theses values, we update them.
	//       that help us to have a good evaluation of the stick range over time.

	void AxisState::SetValue(float in_raw_value, float dead_zone)
	{
		assert(dead_zone >= 0.0f);
		
		if (in_raw_value < dead_zone && in_raw_value > -dead_zone) // apply dead zone to input
			in_raw_value = 0.0f;
		else
			in_raw_value = std::clamp(in_raw_value, -1.0f, +1.0f); // clamp the raw value to -1 .. +1

		// update min and max values
		max_value = std::max(max_value, in_raw_value);
		min_value = std::min(min_value, in_raw_value);

		// normalize input
		if (in_raw_value > 0.0f)
			in_raw_value = (in_raw_value - dead_zone) / (max_value - dead_zone);
		else if (in_raw_value < 0.0f)
			in_raw_value = -(in_raw_value + dead_zone) / (min_value + dead_zone);

		double frame_time = FrameTimeManager::GetInstance()->GetCurrentFrameTime();

		if (update_time < 0.0)  // very first initialization: set both current and previous value to same
		{
			value = in_raw_value;
			update_time = frame_time;
		}
		else 
		{
			auto GetAxisValueType = [](float value)
			{
				if (value == 0.0f)
					return 0;
				if (value < 0.0f)
					return -1;
				return 1;
			};

			if (GetAxisValueType(value) != GetAxisValueType(in_raw_value)) // checking for strict equality for float values is nonsense. just check for 'sign' equality
				update_time = frame_time;

			value = in_raw_value;
		}
	}

}; // namespace chaos
