namespace chaos
{
#ifdef CHAOS_FORWARD_DECLARATION

	template<typename TYPE>
	class DataOwner;

#elif !defined CHAOS_TEMPLATE_IMPLEMENTATION

	// ==============================================================
	// DataOwner
	// ==============================================================

	// the template
	template<typename TYPE>
	class CHAOS_API DataOwner
	{
	public:

		using type = TYPE;

		/** constructor */
		DataOwner() = default;
		/** constructor */
		DataOwner(DataOwner const& src) = default;
		/** constructor */
		DataOwner(type const& in_type) :
			data(in_type) {}

		/** gets the data */
		type& GetData() { return data; }
		/** gets the data */
		type const& GetData() const { return data; }

	public:

		/** the data */
		type data;
	};

	// the empty specialization
	template<>
	class DataOwner<EmptyClass>
	{
	public:
		/** constructor */
		DataOwner() = default;
		/** constructor */
		DataOwner(DataOwner const& src) = default;
		/** constructor */
		DataOwner(EmptyClass const& in_type) {}
	};

	// ==============================================================
	// Functions
	// ==============================================================

	template<typename T, typename U>
	T* GetOwnedData(U* src)
	{
		DataOwner<T>* owner = auto_cast(src);
		if (owner != nullptr)
			return &owner->data;
		return nullptr;
	}

	template<typename T, typename U>
	T const* GetOwnedData(U const* src)
	{
		DataOwner<T> const* owner = auto_cast(src);
		if (owner != nullptr)
			return &owner->data;
		return nullptr;
	}

#endif

}; // namespace chaos