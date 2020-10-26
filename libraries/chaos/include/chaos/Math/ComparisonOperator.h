#ifdef CHAOS_FORWARD_DECLARATION

namespace chaos
{
	enum class ComparisonOperator;

}; // namespace chaos

#else 

namespace chaos
{

	/**
	* ComparisonOperator
	*/

	enum class ComparisonOperator : int
	{
		NEVER,
		ALWAYS,
		EQUAL,
		NOT_EQUAL,
		GREATER,
		GREATER_EQUAL,
		LESS,		
		LESS_EQUAL
	};

	ComparisonOperator operator ! (ComparisonOperator op);

	template<typename T>
	bool Compare(ComparisonOperator op, T const& src1, T const& src2)
	{
		if (op == ComparisonOperator::NEVER)
			return false;
		if (op == ComparisonOperator::ALWAYS)
			return true;
		if (op == ComparisonOperator::EQUAL)
			return (src1 == src2);
		if (op == ComparisonOperator::NOT_EQUAL)
			return (src1 != src2);
		if (op == ComparisonOperator::GREATER)
			return (src1 > src2);
		if (op == ComparisonOperator::GREATER_EQUAL)
			return (src1 >= src2);
		if (op == ComparisonOperator::LESS)
			return (src1 < src2);
		if (op == ComparisonOperator::LESS_EQUAL)
			return (src1 <= src2);
		assert(0);
		return false;
	}

	bool SaveIntoJSON(nlohmann::json& json_entry, ComparisonOperator const& src);

	bool LoadFromJSON(nlohmann::json const& json_entry, ComparisonOperator& dst);

}; // namespace chaos

#endif // CHAOS_FORWARD_DECLARATION


