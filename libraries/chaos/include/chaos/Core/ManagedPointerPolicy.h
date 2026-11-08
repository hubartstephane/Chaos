namespace chaos
{
#ifdef CHAOS_FORWARD_DECLARATION

	class ManagedPointerPolicy;

	template<typename T>
	using managed_ptr = SmartPointer<T, ManagedPointerPolicy>;

	template<typename T>
	struct is_managed_ptr;

#elif !defined CHAOS_TEMPLATE_IMPLEMENTATION

	
	/**
	 * ManagedPointerPolicy: policy used for managed_ptr
	 */

	class CHAOS_API ManagedPointerPolicy
	{
	public:

		/** the internal data in the smart pointer */
		template<typename T>
		using pointer_type = T;

		/** adding a reference */
		template<typename T>
		static T* AddReference(T* in_target)
		{
			assert(in_target != nullptr);
			in_target->AddOwnerReference();
			return in_target;
		}

		/** removing a reference */
		template<typename T>
		static T* SubReference(pointer_type<T>* in_target)
		{
			assert(in_target != nullptr);
			in_target->SubOwnerReference();
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
	* checking whether a data is some kind of managed pointer
	*/

	template<typename T>
	struct is_managed_ptr
	{
		static constexpr bool value = false;
	};

	template<typename T>
	struct is_managed_ptr<managed_ptr<T>>
	{
		static constexpr bool value = true;
	};

	template<typename T>
	constexpr bool is_managed_ptr_v = is_managed_ptr<T>::value;

#endif

}; // namespace chaos

