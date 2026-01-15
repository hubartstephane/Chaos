#include "chaos/ChaosPCH.h"

#include "chaos/ChaosInternals.h"

namespace chaos
{
	InputConditionResult InputConditionBase::Check(InputReceiverInterface const* in_input_receiver, InputDeviceInterface const* in_input_device, InputConsumptionCache& in_consumption_cache) const
	{
		return InputConditionResult::Invalid;
	}

	bool InputConditionBase::IsRequestRelatedTo(Key in_input) const
	{
		return false;
	}

	bool InputConditionBase::IsRequestRelatedTo(Input1D in_input) const
	{
		return false;
	}

	bool InputConditionBase::IsRequestRelatedTo(Input2D in_input) const
	{
		return false;
	}

	char const * InputConditionBase::GetDebugInfo(InputRequestDebugInfoStorage & debug_info_storage) const
	{
		return "";
	}

}; // namespace chaos
