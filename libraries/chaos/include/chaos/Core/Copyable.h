namespace chaos
{

#ifdef CHAOS_FORWARD_DECLARATION

	/** conditionnally disable copy operators for a class */
	template<typename PARENT_CLASS, bool copyable>
	class Copyable;

	/** disable the copy of a class */
	template<typename PARENT_CLASS>
	using NoCopy = Copyable<PARENT_CLASS, false>;

	/** an empty class that can't be copied */
	using NoCopyClass = NoCopy<EmptyClass>;

#elif !defined CHAOS_TEMPLATE_IMPLEMENTATION

	template<typename PARENT_CLASS>
	class /*CHAOS_API*/ Copyable<PARENT_CLASS, true> : public PARENT_CLASS
	{
	};

	template<typename PARENT_CLASS>
	class /*CHAOS_API*/ Copyable<PARENT_CLASS, false> : public PARENT_CLASS
	{
	public:

		Copyable() = default;
		Copyable(Copyable&&) = default;
		Copyable& operator = (Copyable&&) = default;
		Copyable(Copyable const&) = delete;
		Copyable& operator = (Copyable const&) = delete;
	};

#endif

}; // namespace chaos