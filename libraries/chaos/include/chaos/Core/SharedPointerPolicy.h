namespace chaos
{
#ifdef CHAOS_FORWARD_DECLARATION

	class SharedPointerPolicy;

	template<typename T>
	using shared_ptr = SmartPointer<T, SharedPointerPolicy>;

	template<typename T>
	struct is_shared_ptr;

#elif !defined CHAOS_TEMPLATE_IMPLEMENTATION

	/**
	 * SharedPointerPolicy: policy used for shared_ptr
	 */

	class CHAOS_API SharedPointerPolicy
	{
	public:

		/** the internal data in the smart pointer */
		template<typename T>
		using pointer_type = T;

		/** adding a reference */
		template<typename T>
		static T * AddReference(T* in_target)
		{
			assert(in_target != nullptr);
			IntrusiveAddSharedReference(in_target);
			return in_target;
		}

		/** removing a reference */
		template<typename T>
		static T * SubReference(pointer_type<T>* in_target)
		{
			assert(in_target != nullptr);
			IntrusiveSubSharedReference(in_target);
			return nullptr;
		}

		/** get the resource pointed */
		template<typename T>
		static T* Get(T* in_target)
		{
			return in_target;
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

