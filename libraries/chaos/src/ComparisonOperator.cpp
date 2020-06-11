#include <chaos/ComparisonOperator.h>
#include <chaos/JSONTools.h>

namespace chaos
{

	static std::vector<std::pair<ComparisonOperator, char const*>> const comparison_operator_encoding =
	{
		{ ComparisonOperator::never, "never" },
		{ ComparisonOperator::always, "always" },
		{ ComparisonOperator::equal, "equal" },
		{ ComparisonOperator::not_equal, "not_equal" },
		{ ComparisonOperator::greater, "greater" },
		{ ComparisonOperator::greater_equal, "greater_equal" },
		{ ComparisonOperator::less, "less" },
		{ ComparisonOperator::less_equal, "less_equal" }
	};

	ComparisonOperator operator ! (ComparisonOperator op)
	{
		if (op == ComparisonOperator::never)
			return ComparisonOperator::always;
		if (op == ComparisonOperator::always)
			return ComparisonOperator::never;
		if (op == ComparisonOperator::equal)
			return ComparisonOperator::not_equal;
		if (op == ComparisonOperator::not_equal)
			return ComparisonOperator::equal;
		if (op == ComparisonOperator::greater)
			return ComparisonOperator::less_equal;
		if (op == ComparisonOperator::greater_equal)
			return ComparisonOperator::less;
		if (op == ComparisonOperator::less)
			return ComparisonOperator::greater_equal;
		if (op == ComparisonOperator::less_equal)
			return ComparisonOperator::greater;
		assert(0);
		return op;
	}

	CHAOS_IMPLEMENT_ENUMJSON_METHOD(ComparisonOperator, comparison_operator_encoding);

}; // namespace chaos
