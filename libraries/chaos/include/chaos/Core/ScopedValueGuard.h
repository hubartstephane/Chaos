namespace chaos
{
#ifdef CHAOS_FORWARD_DECLARATION

	template<typename T>
	class ScopedValueGuard;

#elif !defined CHAOS_TEMPLATE_IMPLEMENTATION

	/**
	* ValueGuard: this is a RAII class whose purpose is to restore some value at destruction
	*/

	template<typename T>
	class ScopedValueGuard
	{
	public:

		/** constructor */
		ScopedValueGuard(T& in_target):
			target(in_target),
			value(in_target){}

		/** constructor (with assignment) */
		ScopedValueGuard(T& in_target, T const & in_value) :
			target(in_target),
			value(in_target)
		{
			target = in_value;
		}

		/** destructor */
		~ScopedValueGuard()
		{
			target = value;
		}

	protected:

		/** reference to the object to restore */
		T& target;
		/** value to restore */
		T value;
	};

#endif

}; // namespace chaos