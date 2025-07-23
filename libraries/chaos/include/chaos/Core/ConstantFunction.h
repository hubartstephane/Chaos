namespace chaos
{
#ifdef CHAOS_FORWARD_DECLARATION

	template<typename T>
	class ConstantFunctionBase;

	template<typename T>
	using ConstantFunction = ConstantFunctionBase<T>;

	template<typename T>
	using ConstantRefFunction = ConstantFunctionBase<T const &>;

#elif !defined CHAOS_TEMPLATE_IMPLEMENTATION

	/**
	* ConstantFunctionBase: a functor that returns some constant whatever its arguments are
	*/

	template<typename T>
	class ConstantFunctionBase
	{
	public:

		/** constructor */
		ConstantFunctionBase(T  in_value):
			value(in_value)
		{}

		/** copy constructor */
		ConstantFunctionBase(ConstantFunctionBase<T> const & src) = default;

		/** eat arguments and always returns the constant value */
		T const & operator ()(...) const
		{
			return value;
		}

	protected:

		/** constant value */
		T value;
	};

#endif

}; // namespace chaos