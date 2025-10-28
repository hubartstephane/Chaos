#include "chaos/ChaosPCH.h"

#include "chaos/ChaosInternals.h"

namespace chaos
{
	InputRequestResult InputRequestBase::Check(InputReceiverInterface const* in_input_receiver, InputDeviceInterface const* in_input_device, InputConsumptionCache& in_consumption_cache) const
	{
		return InputRequestResult::Invalid;
	}

	bool InputRequestBase::IsRequestRelatedTo(Key in_input) const
	{
		return false;
	}

	bool InputRequestBase::IsRequestRelatedTo(Input1D in_input) const
	{
		return false;
	}

	bool InputRequestBase::IsRequestRelatedTo(Input2D in_input) const
	{
		return false;
	}

	InputRequestDebugInfo InputRequestBase::GetDebugInfo() const
	{
		return {};
	}

}; // namespace chaos
