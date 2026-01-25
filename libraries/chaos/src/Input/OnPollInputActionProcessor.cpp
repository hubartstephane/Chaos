#include "chaos/ChaosPCH.h"

#include "chaos/ChaosInternals.h"

namespace chaos
{
	OnPollInputActionProcessor::OnPollInputActionProcessor(InputReceiverInterface const* in_input_receiver, InputDeviceInterface const* in_input_device, InputConsumptionCache* in_consumption_cache) :
		InputActionProcessor(in_input_receiver, in_input_device),
		consumption_cache(in_consumption_cache)
	{
	}

	bool OnPollInputActionProcessor::CheckAndProcess(InputConditionBase const& in_condition, char const* in_title, InputAction const & in_action)
	{
		if (in_condition.Check({input_receiver, input_device, consumption_cache}) == InputConditionResult::True)
			in_action.Process();
		return false;
	}

}; // namespace chaos
