#include "chaos/ChaosPCH.h"
#include "chaos/ChaosInternals.h"

namespace chaos
{
	static InputRequestResult ComputeAggregationResult(std::optional<InputRequestResult> const& result, InputRequestResult child_result, std::initializer_list<InputRequestResult> const& ordered_values)
	{
		if (result.has_value())
			for (InputRequestResult r : ordered_values)
				if (child_result == r || result.value() == r)
					return r;
		return child_result;
	}

	InputRequestResult AndInputRequestResultAggregation::AggregateResult(std::optional<InputRequestResult> const& result, InputRequestResult child_result)
	{
		return ComputeAggregationResult(result, child_result, {InputRequestResult::Rejected, InputRequestResult::Invalid, InputRequestResult::False, InputRequestResult::True});
	}
	InputRequestResult OrInputRequestResultAggregation::AggregateResult(std::optional<InputRequestResult> const& result, InputRequestResult child_result)
	{
		return ComputeAggregationResult(result, child_result, { InputRequestResult::True, InputRequestResult::False, InputRequestResult::Invalid, InputRequestResult::Rejected });
	}

}; // namespace chaos
