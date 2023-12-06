#include "chaos/ChaosPCH.h"

#include "chaos/ChaosInternals.h"

namespace chaos
{
	//
	// ButtonState functions
	//

	void ButtonState::SetValue(bool in_value)
	{
		previous_value = value;
		value = in_value;
	}

	void ButtonState::UpdateTimerAccumulation(float delta_time)
	{
		if (value != previous_value)
			same_value_timer = 0.0f;
		else
			same_value_timer += delta_time;
	}


	bool ButtonState::IsPressed(bool previous_frame) const
	{
		return GetValue(previous_frame);
	}

	bool ButtonState::IsJustPressed() const
	{
		return IsPressed(false) && !IsPressed(true);
	}

	bool ButtonState::IsJustReleased() const
	{
		return !IsPressed(false) && IsPressed(true);
	}

	ButtonStateChange ButtonState::GetStateChange() const
	{
		bool current_state = IsPressed(false);
		bool previous_state = IsPressed(true);

		if (current_state == previous_state)
			return (current_state) ? ButtonStateChange::STAY_PRESSED : ButtonStateChange::STAY_RELEASED;
		else
			return (current_state) ? ButtonStateChange::BECOME_PRESSED : ButtonStateChange::BECOME_RELEASED;
	}

	//
	// AxisState functions
	//

	// XXX : some sticks are not abled to physicaly returns 1.0 when they are fully triggered (depend on the device)
	//       that's why i use some min/max values (initialized with a coherent value)
	//       if the stick goes further than theses values, we update them.
	//       that help us to have a good evaluation of the stick range over time.

	void AxisState::SetValue(float in_raw_value, float dead_zone)
	{
		previous_value = value;
		value = 0.0f;

		// apply the dead zone
		if (in_raw_value < dead_zone && in_raw_value > -dead_zone)
			return;

		// clamp the raw value to -1 .. +1
		in_raw_value = std::clamp(in_raw_value, -1.0f, +1.0f);

		// store raw value
		max_value = std::max(max_value, in_raw_value);
		min_value = std::min(min_value, in_raw_value);

		// apply dead zone and normalization
		if (in_raw_value > 0.0f)
			value = (in_raw_value - dead_zone) / (max_value - dead_zone);
		else
			value = -(in_raw_value + dead_zone) / (min_value + dead_zone);
	}

	void AxisState::UpdateTimerAccumulation(float delta_time)
	{
		// update timer
		// while floats are never equals (a small stick inclinaison change is detected immedialty),
		// we only check if the sign change (or value become 0 or non 0)
		bool reset_timer =
			(value == 0.0f && previous_value != 0.0f) ||
			(value != 0.0f && previous_value == 0.0f) ||
			(value * previous_value < 0.0f);
		if (reset_timer)
			same_value_timer = 0.0f;
		else
			same_value_timer += delta_time;
	}

}; // namespace chaos
