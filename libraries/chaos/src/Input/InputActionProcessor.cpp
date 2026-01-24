#include "chaos/ChaosPCH.h"

#include "chaos/ChaosInternals.h"

namespace chaos
{
	InputActionProcessor::InputActionProcessor(InputReceiverInterface const* in_input_receiver, InputDeviceInterface const* in_input_device) :
		input_receiver(in_input_receiver),
		input_device(in_input_device)
	{
		assert(in_input_receiver != nullptr);
		assert(in_input_device != nullptr);
	}

	bool InputActionProcessor::CheckAndProcess(InputConditionBase const & in_condition, char const * in_title, InputActionFunction in_action_func)
	{
		return CheckAndProcess(in_condition, in_title, true, in_action_func);
	}

	bool InputActionProcessor::CheckAndProcess(InputConditionBase const& in_condition, char const* in_title, bool in_enabled, InputActionFunction in_action_func)
	{
		return false;
	}

	void InputActionProcessor::MarkAllRequestInputsAsConsumedInApplicationCache(InputConditionBase const& in_condition)
	{
		if (WindowApplication* window_application = Application::GetInstance())
			in_condition.Check({nullptr, input_device, &window_application->GetInputConsumptionCache()});
	}

}; // namespace chaos
