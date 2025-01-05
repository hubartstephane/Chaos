namespace chaos
{
#ifdef CHAOS_FORWARD_DECLARATION

	class Default;

#elif !defined CHAOS_TEMPLATE_IMPLEMENTATION

	/**
	 * Default: this is class used to create any object on demand
	 * 
	 * note: this is not possible to use {} syntax for passing arguments to varidic functions
	 *       because the compiler has no way to understand what object is expected
	 *       Default() is here to solve the issue
	 */
	class Default
	{
	public:

		/** create any default constructible object */
		template<typename T>
		operator T ()
		{
			return {};
		}
	};

#endif

}; // namespace chaos