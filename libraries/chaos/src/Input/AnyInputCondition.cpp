#include "chaos/ChaosPCH.h"

#include "chaos/ChaosInternals.h"

namespace chaos
{
	char const * AnyInputCondition::GetDebugInfo(InputConditionDebugInfoStorage & debug_info_storage) const
	{
		return "Consume All Inputs";
	}

	bool AnyInputCondition::IsRequestRelatedTo(Key in_input) const
	{
		return true;
	}

	bool AnyInputCondition::IsRequestRelatedTo(Input1D in_input) const
	{
		return true;
	}

	bool AnyInputCondition::IsRequestRelatedTo(Input2D in_input) const
	{
		return true;
	}

	InputConditionResultType AnyInputCondition::Check(InputConditionCheckParams const& in_params) const
	{
		in_params.consumption_cache->SetAllInputConsumer(in_params.input_receiver);

		return in_params.input_device->IsAnyInputActive()?
			InputConditionResultType::True:
			InputConditionResultType::False;
	}

}; // namespace chaos
