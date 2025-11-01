#include "chaos/ChaosPCH.h"

#include "chaos/ChaosInternals.h"

namespace chaos
{
	InputActionEnumerator::InputActionEnumerator(InputReceiverInterface const* in_input_receiver, InputDeviceInterface const* in_input_device) :
		input_receiver(in_input_receiver),
		input_device(in_input_device)
	{
		assert(in_input_receiver != nullptr);
		assert(in_input_device != nullptr);
	}

	bool InputActionEnumerator::CheckAndProcess(InputRequestBase const & in_request, char const * in_title, InputActionFunction in_action_func)
	{
		return CheckAndProcess(in_request, in_title, true, in_action_func);
	}

	bool InputActionEnumerator::CheckAndProcess(InputRequestBase const& in_request, char const* in_title, bool in_enabled, InputActionFunction in_action_func)
	{
		return false;
	}

	void InputActionEnumerator::MarkAllRequestInputsAsConsumedInApplicationCache(InputRequestBase const& in_request)
	{
		if (WindowApplication* window_application = Application::GetInstance())
			in_request.Check(nullptr, input_device, window_application->GetInputConsumptionCache());
	}

}; // namespace chaos
