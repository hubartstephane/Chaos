namespace chaos
{
#ifdef CHAOS_FORWARD_DECLARATION

	enum class ComparisonOperator;

#elif !defined CHAOS_TEMPLATE_IMPLEMENTATION

	/**
	* ComparisonOperator
	*/

	enum class ComparisonOperator : int
	{
		Never,
		Always,
		Equal,
		NotEqual,
		Greater,
		GreaterEqual,
		Less,
		LessEqual
	};

	CHAOS_API ComparisonOperator operator ! (ComparisonOperator op);

	template<typename T>
	bool Compare(ComparisonOperator op, T const& src1, T const& src2)
	{
		if (op == ComparisonOperator::Never)
			return false;
		if (op == ComparisonOperator::Always)
			return true;
		if (op == ComparisonOperator::Equal)
			return (src1 == src2);
		if (op == ComparisonOperator::NotEqual)
			return (src1 != src2);
		if (op == ComparisonOperator::Greater)
			return (src1 > src2);
		if (op == ComparisonOperator::GreaterEqual)
			return (src1 >= src2);
		if (op == ComparisonOperator::Less)
			return (src1 < src2);
		if (op == ComparisonOperator::LessEqual)
			return (src1 <= src2);
		assert(0);
		return false;
	}

	CHAOS_DECLARE_ENUM_METHOD(ComparisonOperator, CHAOS_API);

#endif

}; // namespace chaos