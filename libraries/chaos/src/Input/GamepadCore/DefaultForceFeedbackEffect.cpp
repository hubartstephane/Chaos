#include "chaos/ChaosPCH.h"
#include "chaos/ChaosInternals.h"

namespace chaos
{
	bool DefaultForceFeedbackEffect::TickAndGetMotorValues(float delta_time, ForceFeedbackMotorValues& out_motor_values)
	{
		if (duration > 0.0f)
		{
			timer += delta_time;
			if (timer >= duration)
			{
				out_motor_values = {};
				return true; // end of the effect
			}
		}
		out_motor_values = motor_values;
		return false; // do not destroy the effect
	}

}; // namespace chaos
