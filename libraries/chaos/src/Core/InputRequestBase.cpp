#include "chaos/ChaosPCH.h"

#include "chaos/ChaosInternals.h"

namespace chaos
{
	InputRequestResult InputRequestBase::Check(InputEventReceiverInterface const* in_event_receiver, KeyEventBase const& in_key_event, InputDeviceInterface const* in_input_device, InputConsumptionCache & in_consumption_cache) const
	{
		return InputRequestResult::Invalid;
	}

	InputRequestResult InputRequestBase::Check(InputEventReceiverInterface const* in_event_receiver, InputDeviceInterface const* in_input_device, InputConsumptionCache & in_consumption_cache) const
	{
		return InputRequestResult::Invalid;
	}

}; // namespace chaos
