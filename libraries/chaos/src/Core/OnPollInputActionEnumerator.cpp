#include "chaos/ChaosPCH.h"

#include "chaos/ChaosInternals.h"

namespace chaos
{
	OnPollInputActionEnumerator::OnPollInputActionEnumerator(InputEventReceiverInterface const* in_event_receiver, InputDeviceInterface const* in_input_device, InputConsumptionCache* in_consumption_cache) :
		InputActionEnumerator(in_event_receiver),
		input_device(in_input_device),
		consumption_cache(in_consumption_cache)
	{
	}

	bool OnPollInputActionEnumerator::CheckAndProcess(InputRequestBase const& in_request, char const* in_title, bool in_enabled, InputActionFunction in_func)
	{
		if (in_request.Check(event_receiver, input_device, *consumption_cache) == InputRequestResult::True)
		{
			if (in_enabled)
				in_func(); // continue to next request
		}
		return false;
	}

}; // namespace chaos
