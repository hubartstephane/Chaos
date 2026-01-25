#include "chaos/ChaosPCH.h"

#include "chaos/ChaosInternals.h"

namespace chaos
{
	InputActionProcessor::InputActionProcessor(InputReceiverInterface const* in_input_receiver, InputDeviceInterface const* in_input_device, InputConsumptionCache* in_consumption_cache) :
		input_receiver(in_input_receiver),
		input_device(in_input_device),
		consumption_cache(in_consumption_cache)
	{
		assert(in_input_receiver != nullptr);
		assert(in_input_device != nullptr);
		assert(in_consumption_cache != nullptr);
	}

	bool InputActionProcessor::CheckAndProcess(InputConditionBase const& in_condition, char const* in_title, InputAction const & in_action)
	{
		return false;
	}

}; // namespace chaos
