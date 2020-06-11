#pragma once

#include <chaos/StandardHeaders.h>
#include <chaos/EnumTools.h>
#include <chaos/JSONTools.h>

namespace chaos
{

	/**
	* ComparisonOperator
	*/

	enum class ComparisonOperator : int
	{
		never,
		always,
		equal,
		not_equal,
		greater,
		greater_equal,
		less,		
		less_equal
	};

	ComparisonOperator operator ! (ComparisonOperator op);

	template<typename T>
	bool Compare(ComparisonOperator op, T const& src1, T const& src2)
	{
		if (op == ComparisonOperator::never)
			return false;
		if (op == ComparisonOperator::always)
			return true;
		if (op == ComparisonOperator::equal)
			return (src1 == src2);
		if (op == ComparisonOperator::not_equal)
			return (src1 != src2);
		if (op == ComparisonOperator::greater)
			return (src1 > src2);
		if (op == ComparisonOperator::greater_equal)
			return (src1 >= src2);
		if (op == ComparisonOperator::less)
			return (src1 < src2);
		if (op == ComparisonOperator::less_equal)
			return (src1 <= src2);
		assert(0);
		return false;
	}

	bool SaveIntoJSON(nlohmann::json& json_entry, ComparisonOperator const& src);

	bool LoadFromJSON(nlohmann::json const& json_entry, ComparisonOperator& dst);

}; // namespace chaos
