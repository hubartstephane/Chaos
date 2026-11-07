namespace chaos
{
#ifdef CHAOS_FORWARD_DECLARATION

	class SharedPointerPolicy;

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
			::intrusive_ptr_add_ref(in_target);
			return in_target;
		}

		/** removing a reference */
		template<typename T>
		static T * SubReference(pointer_type<T>* in_target)
		{
			assert(in_target != nullptr);
			::intrusive_ptr_release(in_target);
			return nullptr;
		}

		/** get the resource pointed */
		template<typename T>
		static T* Get(T* in_target)
		{
			return in_target;
		}
	};

#endif

}; // namespace chaos

