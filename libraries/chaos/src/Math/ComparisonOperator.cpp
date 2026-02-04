#include "chaos/ChaosPCH.h"
#include "chaos/ChaosInternals.h"

namespace chaos
{

	static EnumMetaData<ComparisonOperator> const ComparisonOperator_metadata =
	{
		{ ComparisonOperator::Never, "NEVER" },
		{ ComparisonOperator::Always, "ALWAYS" },
		{ ComparisonOperator::Equal, "EQUAL" },
		{ ComparisonOperator::NotEqual, "NOT_EQUAL" },
		{ ComparisonOperator::Greater, "GREATER" },
		{ ComparisonOperator::GreaterEqual, "GREATER_EQUAL" },
		{ ComparisonOperator::Less, "LESS" },
		{ ComparisonOperator::LessEqual, "LESS_EQUAL" }
	};

	ComparisonOperator operator ! (ComparisonOperator op)
	{
		if (op == ComparisonOperator::Never)
			return ComparisonOperator::Always;
		if (op == ComparisonOperator::Always)
			return ComparisonOperator::Never;
		if (op == ComparisonOperator::Equal)
			return ComparisonOperator::NotEqual;
		if (op == ComparisonOperator::NotEqual)
			return ComparisonOperator::Equal;
		if (op == ComparisonOperator::Greater)
			return ComparisonOperator::LessEqual;
		if (op == ComparisonOperator::GreaterEqual)
			return ComparisonOperator::Less;
		if (op == ComparisonOperator::Less)
			return ComparisonOperator::GreaterEqual;
		if (op == ComparisonOperator::LessEqual)
			return ComparisonOperator::Greater;
		assert(0);
		return op;
	}

	CHAOS_IMPLEMENT_ENUM_METHOD(ComparisonOperator, &ComparisonOperator_metadata, CHAOS_API);

}; // namespace chaos
