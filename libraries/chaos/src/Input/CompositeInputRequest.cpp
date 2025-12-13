#include "chaos/ChaosPCH.h"
#include "chaos/ChaosInternals.h"

namespace chaos
{
	namespace details
	{
		char FindAccessibleOperator(char const* buffer)
		{
			size_t i = 0;
			while (buffer[i] != 0)
			{
				char c = buffer[i];

				// operator caracter found
				if (c == '&' || c == '|')
					return c;

				// very first opening parenthesis -> search corresponding closing parenthesis (and skip internal parenthesis)
				if (c == '(')
				{
					size_t parenthesis_count = 1;
					++i;
					while (buffer[i] != 0)
					{
						char c2 = buffer[i];
						if (c2 == ')')
						{
							--parenthesis_count;
							if (parenthesis_count == 0)
								break;
						}
						else if (c2 == '(')
						{
							++parenthesis_count;
						}
						++i;
						if (buffer[i] == 0)
							return 0;
					}
				}
				++i;
			}
			return 0;
		}

	}; // namespace details

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
