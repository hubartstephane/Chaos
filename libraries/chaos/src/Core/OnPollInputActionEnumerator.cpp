#include "chaos/ChaosPCH.h"

#include "chaos/ChaosInternals.h"

namespace chaos
{
	OnPollInputActionEnumerator::OnPollInputActionEnumerator(InputDeviceInterface const* in_input_device, InputConsumptionCache* in_consumption_cache) :
		input_device(in_input_device),
		consumption_cache(in_consumption_cache)
	{
	}

	bool OnPollInputActionEnumerator::CheckAndProcess(InputRequestBase const& in_request, char const* in_title, bool in_enabled, InputActionFunction in_func)
	{
		if (in_request.Check(input_device, *consumption_cache))
		{
			if (in_enabled)
				in_func(); // continue to next request
		}
		return false;
	}

}; // namespace chaos
