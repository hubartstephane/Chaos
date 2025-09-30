#include "chaos/ChaosPCH.h"

#include "chaos/ChaosInternals.h"

namespace chaos
{
	OnKeyEventInputActionEnumerator::OnKeyEventInputActionEnumerator(InputEventReceiverInterface const* in_event_receiver, KeyEventBase const& in_key_event, InputDeviceInterface const* in_input_device, InputConsumptionCache* in_consumption_cache) :
		InputActionEnumerator(in_event_receiver),
		key_event(in_key_event),
		input_device(in_input_device),
		consumption_cache(in_consumption_cache)
	{
	}

	bool OnKeyEventInputActionEnumerator::CheckAndProcess(InputRequestBase const& in_request, char const* in_title, bool in_enabled, InputActionFunction in_func)
	{
		if (in_request.Check(event_receiver, key_event, input_device, *consumption_cache) == InputRequestResult::True)
		{
			if (in_enabled)
				in_func();
			return true; // stop
		}
		return false;
	}

}; // namespace chaos
