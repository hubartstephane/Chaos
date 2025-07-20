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
		return GetValue();
	}

	bool ButtonState::IsUp() const
	{
		return !IsDown();
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

	ButtonStateChange ButtonState::GetStateChange() const
	{
		float same_value_time = GetSameValueTimer();

		if (IsDown())
		{
			if (same_value_time == 0.0f)
				return ButtonStateChange::BECOME_PRESSED;
			else
				return ButtonStateChange::STAY_PRESSED;
		}
		else if (IsUp())
		{
			if (same_value_time == 0.0f)
				return ButtonStateChange::BECOME_RELEASED;
			else
				return ButtonStateChange::STAY_RELEASED;
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
