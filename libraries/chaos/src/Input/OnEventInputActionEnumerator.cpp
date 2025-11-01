#include "chaos/ChaosPCH.h"

#include "chaos/ChaosInternals.h"

namespace chaos
{
	OnEventInputActionEnumerator::OnEventInputActionEnumerator(InputReceiverInterface const* in_input_receiver, InputDeviceInterface const* in_input_device, KeyEventBase const& in_key_event, InputConsumptionCache* in_consumption_cache) :
		InputActionEnumerator(in_input_receiver, in_input_device),
		key_event(in_key_event),
		consumption_cache(in_consumption_cache)
	{
	}

	bool OnEventInputActionEnumerator::CheckAndProcess(InputRequestBase const& in_request, char const* in_title, bool in_enabled, InputActionFunction in_func)
	{
		// always consum input even if not related because, later on, maybe some composite request with the related key 
		// and some consumed input will be checked
		if (in_request.Check(input_receiver, input_device, *consumption_cache) == InputRequestResult::True)
		{
			if (in_request.IsRequestRelatedTo(key_event.key)) // ignore all Actions with no relation with the handled event
			{
				MarkAllRequestInputsAsConsumed(in_request);

				if (in_enabled && in_func.IsValid())
					in_func();
				return true; // stop
			}
		}
		return false;
	}

}; // namespace chaos
