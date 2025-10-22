#include "chaos/ChaosPCH.h"
#include "chaos/ChaosInternals.h"

namespace chaos
{
	InputRequestResult AndInputRequestResultAggregation::AggregateResult(std::optional<InputRequestResult> const& result, InputRequestResult child_result)
	{
		if (!result.has_value())
			return child_result;
		if (child_result == InputRequestResult::Rejected || result == InputRequestResult::Rejected)
			return InputRequestResult::Rejected;
		if (child_result == InputRequestResult::Invalid || result == InputRequestResult::Invalid)
			return InputRequestResult::Invalid;
		if (child_result == InputRequestResult::False || result == InputRequestResult::False)
			return InputRequestResult::False;
		return InputRequestResult::True;
	}
	InputRequestResult OrInputRequestResultAggregation::AggregateResult(std::optional<InputRequestResult> const& result, InputRequestResult child_result)
	{
		if (!result.has_value())
			return child_result;
		if (child_result == InputRequestResult::True || result == InputRequestResult::True)
			return InputRequestResult::True;
		if (child_result == InputRequestResult::Rejected || result == InputRequestResult::Rejected)
			return InputRequestResult::Rejected;
		if (child_result == InputRequestResult::Invalid || result == InputRequestResult::Invalid)
			return InputRequestResult::Invalid;
		return InputRequestResult::False;
	}

}; // namespace chaos
