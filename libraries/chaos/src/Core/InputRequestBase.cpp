#include "chaos/ChaosPCH.h"

#include "chaos/ChaosInternals.h"

namespace chaos
{
	bool InputRequestBase::Check(KeyEventBase const& in_key_event, InputDeviceInterface const* in_input_device, InputConsumptionCache & in_consumption_cache) const
	{
		return false;
	}

	bool InputRequestBase::Check(InputDeviceInterface const* in_input_device, InputConsumptionCache & in_consumption_cache) const
	{
		return false;
	}

}; // namespace chaos
