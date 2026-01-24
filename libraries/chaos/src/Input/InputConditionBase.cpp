#include "chaos/ChaosPCH.h"

#include "chaos/ChaosInternals.h"

namespace chaos
{
	InputConditionResult InputConditionBase::Check(InputConditionCheckParams const& in_params) const
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

	char const * InputConditionBase::GetDebugInfo(InputConditionDebugInfoStorage & debug_info_storage) const
	{
		return "";
	}

}; // namespace chaos
