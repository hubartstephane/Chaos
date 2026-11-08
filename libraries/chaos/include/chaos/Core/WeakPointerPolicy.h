namespace chaos
{
#ifdef CHAOS_FORWARD_DECLARATION

	class WeakPointerData;
	class WeakPointerPolicy;

	template<typename T>
	using weak_ptr = SmartPointer<T, WeakPointerPolicy>;

	template<typename T>
	struct is_weak_ptr;

#elif !defined CHAOS_TEMPLATE_IMPLEMENTATION

	//   weak_ptr<> use an intermediate structure that points on the target object
	//
	//   weak_ptr
	//       |
	//       +-----> WeakPointerData <-----+
	//                     |               |
	//                     +----> target --+
	//
	//    to work the target must have a pointer on the WeakPointerData so it can reset it when it is destroyed
	//
	//    As a side effect, accessing target from a weak_ptr requires an additionnal indirection (slower)
	//                      using weak_ptr cause an allocation (for the very first time only)

	/**
	 * WeakPointerData : an utility structure used to handle weak pointers
	 */

	class CHAOS_API WeakPointerData
	{
	public:

		/** constructor */
		WeakPointerData(void const* ptr) : // XXX : const here because a weak_ptr<T const> make sense
			object_ptr(ptr)
		{
			assert(ptr != nullptr);
		}

		/** pointer on the object */
		void const* object_ptr = nullptr;
		/** count weak reference */
		std::atomic<int> weak_ref_count;
	};

	/**
	 * WeakPointerPolicy : policy used for weak_ptr
	 */

	class CHAOS_API WeakPointerPolicy
	{
	public:

		/** the internal data in the smart pointer */
		template<typename T>
		using pointer_type = WeakPointerData;

		/** adding a reference */
		template<typename T>
		static WeakPointerData* AddReference(T* in_target)
		{
			assert(in_target != nullptr);

			if (in_target->weak_ptr_data == nullptr) // reserve a weak structure if needed
			{
				in_target->weak_ptr_data = new WeakPointerData(in_target);
				if (in_target->weak_ptr_data == nullptr)
					return nullptr;
			}
			++in_target->weak_ptr_data->weak_ref_count;
			return in_target->weak_ptr_data;
		}

		/** removing a reference */
		static WeakPointerData* SubReference(WeakPointerData* in_target)
		{
			--in_target->weak_ref_count;
			return nullptr;
		}

		/** get the resource pointed */
		template<typename T>
		static T* Get(WeakPointerData* in_target)
		{
			if (in_target != nullptr)
				return (T*)in_target->object_ptr; // remove constness here
			return nullptr;
		}
	};

	/**
	* checking whether a data is some kind of weak pointer
	*/

	template<typename T>
	struct is_weak_ptr
	{
		static constexpr bool value = false;
	};

	template<typename T>
	struct is_weak_ptr<weak_ptr<T>>
	{
		static constexpr bool value = true;
	};

	template<typename T>
	constexpr bool is_weak_ptr_v = is_weak_ptr<T>::value;

#endif

}; // namespace chaos
