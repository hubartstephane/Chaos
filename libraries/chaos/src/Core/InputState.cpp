#include "chaos/ChaosPCH.h"

#include "chaos/ChaosInternals.h"

namespace chaos
{
	//
	// ButtonState functions
	//

	void ButtonState::SetValue(bool in_value)
	{
		double frame_time = FrameTimeManager::GetInstance()->GetCurrentFrameTime();

		if (update_time < 0.0) // very first initialization: set both current and previous value to same
		{
			previous_value = in_value;
			value = in_value;
			update_time = frame_time;
		}
		else if (value != in_value)
		{
			if (frame_time != update_time)
			{
				previous_value = value; // do not override previous_value if we are updating the button state in the same frame
				update_time = frame_time;
			}
			value = in_value;
		}
	}

	bool ButtonState::IsPressed(InputStateFrame frame) const
	{
		return GetValue(frame);
	}

	bool ButtonState::IsJustPressed() const
	{
		return IsPressed(InputStateFrame::CURRENT) && !IsPressed(InputStateFrame::PREVIOUS);
	}

	bool ButtonState::IsJustReleased() const
	{
		return !IsPressed(InputStateFrame::CURRENT) && IsPressed(InputStateFrame::PREVIOUS);
	}

	ButtonStateChange ButtonState::GetStateChange() const
	{
		bool current_state = IsPressed(InputStateFrame::CURRENT);
		bool previous_state = IsPressed(InputStateFrame::PREVIOUS);

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
		else
			in_raw_value = -(in_raw_value + dead_zone) / (min_value + dead_zone);

		double frame_time = FrameTimeManager::GetInstance()->GetCurrentFrameTime();

		if (update_time < 0.0)  // very first initialization: set both current and previous value to same
		{
			previous_value = in_raw_value;
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
			{
				if (frame_time != update_time)
				{
					previous_value = value; // do not override previous_value if we are updating the button state in the same frame
					update_time = frame_time;
				}
			}
			value = in_raw_value;
		}
	}

}; // namespace chaos
