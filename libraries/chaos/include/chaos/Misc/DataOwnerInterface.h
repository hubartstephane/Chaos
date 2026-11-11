namespace chaos
{
#ifdef CHAOS_FORWARD_DECLARATION

	template<typename TYPE>
	class DataOwnerInterface;

#elif !defined CHAOS_TEMPLATE_IMPLEMENTATION

	/**
	 * DataOwnerInterface: an interface designed to just to have a getter on some data
	 **/

	template<typename TYPE>
	class DataOwnerInterface : public virtual Interface
	{
	public:

		using type = TYPE;

		/** constructor */
		DataOwnerInterface() = default;
		/** constructor */
		DataOwnerInterface(DataOwnerInterface const& src) = default;
		/** constructor */
		DataOwnerInterface(type const& in_type) :
			data(in_type) {}

		/** gets the data */
		type& GetData() { return data; }
		/** gets the data */
		type const& GetData() const { return data; }

	protected:

		/** the data */
		type data;
	};

	// the empty specialization
	template<>
	class DataOwnerInterface<EmptyClass> : public virtual Interface
	{
	public:
		/** constructor */
		DataOwnerInterface() = default;
		/** constructor */
		DataOwnerInterface(DataOwnerInterface const& src) = default;
		/** constructor */
		DataOwnerInterface(EmptyClass const& in_type) {}
	};

	/**
	 * Functions
	 */

	template<typename T, typename U>
	T* GetOwnedData(U* src)
	{
		DataOwnerInterface<T>* owner = auto_cast(src);
		if (owner != nullptr)
			return &owner->GetData();
		return nullptr;
	}

	template<typename T, typename U>
	T const* GetOwnedData(U const* src)
	{
		DataOwnerInterface<T> const* owner = auto_cast(src);
		if (owner != nullptr)
			return &owner->GetData();
		return nullptr;
	}

#endif

}; // namespace chaos