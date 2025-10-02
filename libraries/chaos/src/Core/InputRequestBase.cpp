#include "chaos/ChaosPCH.h"

#include "chaos/ChaosInternals.h"

namespace chaos
{
	std::string InputRequestBase::GetInputTitle() const
	{
		return {};
	}

	InputRequestResult InputRequestBase::Check(InputReceiverInterface const* in_input_receiver, KeyEventBase const& in_key_event, InputDeviceInterface const* in_input_device, InputConsumptionCache & in_consumption_cache) const
	{
		return InputRequestResult::Invalid;
	}

	InputRequestResult InputRequestBase::Check(InputReceiverInterface const* in_input_receiver, InputDeviceInterface const* in_input_device, InputConsumptionCache & in_consumption_cache) const
	{
		return InputRequestResult::Invalid;
	}

}; // namespace chaos
