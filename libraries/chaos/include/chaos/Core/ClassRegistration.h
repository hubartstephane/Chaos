namespace chaos
{
#ifdef CHAOS_FORWARD_DECLARATION

	class ClassRegistration;

#elif !defined CHAOS_TEMPLATE_IMPLEMENTATION

	/**
	 * ClassRegistration: an intermediate result class (that bring possibility to add extra information to the class)
	 */

	class CHAOS_API ClassRegistration
	{
		friend class ClassManager;

	protected:

		/** constructor (may only be called from Class) */
		ClassRegistration(Class* in_class_ptr) :
			class_ptr(in_class_ptr)
		{
			assert(in_class_ptr != nullptr);
		}

	public:

		/** set the short name for the class */
		ClassRegistration & operator()(std::string in_short_name);

		/** convert the registration to the class */
		operator Class const * () const
		{
			return class_ptr;
		}

	protected:

		/** the class */
		Class * class_ptr = nullptr;
	};

#endif

}; // namespace chaos