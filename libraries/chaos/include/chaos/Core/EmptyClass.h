#ifdef CHAOS_FORWARD_DECLARATION

namespace chaos
{
	/** an empty base class */
	using EmptyClass = boost::mpl::empty_base;

	class EmptyClassVTable;

	/** a selector for EmpyClass with or without VTABLE */
	template<bool VTABLE = false>
	using EmptyClassExt = typename boost::mpl::if_c<VTABLE, EmptyClassVTable, EmptyClass>::type;

}; // namespace chaos

#elif defined CHAOS_TEMPLATE_IMPLEMENTATION


#else 

namespace chaos
{

	/** an 'empty' base class with a VTABLE */
	class EmptyClassVTable
	{
	public:

		virtual ~EmptyClassVTable() = default;
	};

}; // namespace chaos

#endif // CHAOS_FORWARD_DECLARATION
