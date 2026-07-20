namespace chaos
{
#ifdef CHAOS_FORWARD_DECLARATION

	template<typename CPP_TYPE>
	class ClassRegistrationResult;

#elif !defined CHAOS_TEMPLATE_IMPLEMENTATION

	/**
	 * ClassRegistrationResult: utility class used as a result when a class is registered inside a manager (and bring the possibility to make extra initialization)
	 */

	template<typename CPP_TYPE>
	class ClassRegistrationResult
	{
	public:

		/** constructor */
		ClassRegistrationResult(Class<CPP_TYPE>* in_target_class = nullptr) :
			target_class(in_target_class) {
		}
		/** set the short name for the class */
		ClassRegistrationResult& ShortName(std::string in_short_name)
		{
			target_class->SetShortName(std::move(in_short_name));
			return *this;
		}
		/** convert the registration to the class */
		operator Class<CPP_TYPE>* () const
		{
			return target_class;
		}
		/** convert the registration to the class */
		Class<CPP_TYPE>* operator -> () const
		{
			return target_class;
		}

	protected:

		/** the newly registered class */
		Class<CPP_TYPE>* target_class = nullptr;
	};

#endif

}; // namespace chaos