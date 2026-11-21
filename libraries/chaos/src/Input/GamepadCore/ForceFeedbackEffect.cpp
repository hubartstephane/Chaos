#include "chaos/ChaosPCH.h"
#include "chaos/ChaosInternals.h"

namespace chaos
{
	ForceFeedbackEffect::~ForceFeedbackEffect()
	{
		RemoveFromGamepad();
	}

	void ForceFeedbackEffect::RemoveFromSuperviser()
	{
		RemoveFromGamepad();
	}

	bool ForceFeedbackEffect::TickAndGetMotorValues(float delta_time, ForceFeedbackMotorValues& out_motor_values)
	{
		out_motor_values = {};
		return false; // do not destroy the effect
	}

	void ForceFeedbackEffect::RemoveFromGamepad()
	{
		if (gamepad == nullptr)
			return;
		gamepad->RemoveForceFeedbackEffect(this);
	}

}; // namespace chaos
