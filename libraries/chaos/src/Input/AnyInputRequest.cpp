#include "chaos/ChaosPCH.h"

#include "chaos/ChaosInternals.h"

namespace chaos
{
	InputRequestDebugInfo AnyInputRequest::GetDebugInfo() const
	{
		InputRequestDebugInfo result;
		result.input = "All Inputs";
		result.action_type = "Consume all";
		return result;
	}

	bool AnyInputRequest::IsRequestRelatedTo(Key in_input) const
	{
		return true;
	}

	bool AnyInputRequest::IsRequestRelatedTo(Input1D in_input) const
	{
		return true;
	}

	bool AnyInputRequest::IsRequestRelatedTo(Input2D in_input) const
	{
		return true;
	}

	InputRequestResult AnyInputRequest::Check(InputReceiverInterface const* in_input_receiver, InputDeviceInterface const* in_input_device, InputConsumptionCache & in_consumption_cache) const
	{
		in_consumption_cache.SetConsumeAllInputs(in_input_receiver);

		return in_input_device->IsAnyInputActive()?
			InputRequestResult::True:
			InputRequestResult::False;
	}

}; // namespace chaos
