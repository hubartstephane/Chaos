namespace chaos
{
#ifdef CHAOS_FORWARD_DECLARATION

	template<typename T>
	class SharedPointerReferenceCounter;

	template<typename T>
	using SharedPointerData = OwnerPointerDataBase<T, SharedPointerReferenceCounter>;

	template<typename T>
	using shared_ptr = SmartPointer<T, SharedPointerData>;

	template<typename T>
	struct is_shared_ptr;

#elif !defined CHAOS_TEMPLATE_IMPLEMENTATION

	/**
	 * SharedPointerReferenceCounter: a class responsible for adding/removing shared counters
	 */

	template<typename T>
	class SharedPointerReferenceCounter
	{
	public:

		/** adding a reference to a target object */
		static void AddReferenceToTarget(T* in_target)
		{
			IntrusiveAddSharedReference(in_target);
		}
		/** removing a reference to a target object */
		static void SubReferenceToTarget(T* in_target)
		{
			IntrusiveSubSharedReference(in_target);
		}
	};

	/**
	* checking whether a data is some kind of shared pointer
	*/

	template<typename T>
	struct is_shared_ptr
	{
		static constexpr bool value = false;
	};

	template<typename T>
	struct is_shared_ptr<shared_ptr<T>>
	{
		static constexpr bool value = true;
	};

	template<typename T>
	constexpr bool is_shared_ptr_v = is_shared_ptr<T>::value;

#endif

}; // namespace chaos

