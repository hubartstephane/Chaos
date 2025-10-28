#include "chaos/ChaosPCH.h"

#include "chaos/ChaosInternals.h"

namespace chaos
{
	OnKeyEventInputActionEnumerator::OnKeyEventInputActionEnumerator(InputReceiverInterface const* in_input_receiver, KeyEventBase const& in_key_event, InputDeviceInterface const* in_input_device, InputConsumptionCache* in_consumption_cache) :
		InputActionEnumerator(in_input_receiver),
		key_event(in_key_event),
		input_device(in_input_device),
		consumption_cache(in_consumption_cache)
	{
	}

	bool OnKeyEventInputActionEnumerator::CheckAndProcess(InputRequestBase const& in_request, char const* in_title, bool in_enabled, InputActionFunction in_func)
	{
		if (in_request.Check(input_receiver, input_device, *consumption_cache) == InputRequestResult::True)
		{
			if (in_request.IsRequestRelatedTo(key_event.key)) // ignore all Actions with no relation with the handled event
			{
				if (in_enabled && in_func.IsValid())
					in_func();
				return true; // stop
			}
		}
		return false;
	}

}; // namespace chaos
