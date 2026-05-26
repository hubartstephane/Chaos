namespace chaos
{
#ifdef CHAOS_FORWARD_DECLARATION

	template<typename CLASS_TYPE>
	class ClassRegistration;

#elif !defined CHAOS_TEMPLATE_IMPLEMENTATION

	/**
	 * ClassRegistration: an intermediate result class (that bring possibility to add extra information to the class)
	 */

	template<typename CLASS_TYPE>
	class ClassRegistration
	{
		friend class ClassManager;

	protected:

		/** constructor (may only be called from Class) */
		ClassRegistration(Class< CLASS_TYPE> * in_class_ptr) :
			class_ptr(in_class_ptr)
		{
			assert(in_class_ptr != nullptr);
		}

	public:

		/** set the short name for the class */
		ClassRegistration & ShortName(std::string in_short_name)
		{
			class_ptr->SetShortName(std::move(in_short_name));
			return *this;
		}

		/** convert the registration to the class */
		operator Class<CLASS_TYPE> const * () const
		{
			return class_ptr;
		}

	protected:

		/** the class */
		Class<CLASS_TYPE> * class_ptr = nullptr;
	};

#endif

}; // namespace chaos