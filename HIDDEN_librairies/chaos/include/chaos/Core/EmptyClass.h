namespace chaos
{
#ifdef CHAOS_FORWARD_DECLARATION

	/** an empty base class */
	using EmptyClass = boost::mpl::empty_base;

	class EmptyClassVTable;

	/** a selector for EmpyClass with or without VTABLE */
	template<bool VTABLE = false>
	using EmptyClassExt = typename std::conditional<VTABLE, EmptyClassVTable, EmptyClass>::type;

#elif !defined CHAOS_TEMPLATE_IMPLEMENTATION

	/** an 'empty' base class with a VTABLE */
	class CHAOS_API EmptyClassVTable
	{
	public:

		virtual ~EmptyClassVTable() = default;
	};

#endif

}; // namespace chaos