namespace chaos
{
#ifdef CHAOS_FORWARD_DECLARATION

	class SupervisedPointerPolicy;

	template<typename T>
	using supervised_ptr = SmartPointer<T, SupervisedPointerPolicy>;

	template<typename T>
	struct is_supervised_ptr;

#elif !defined CHAOS_TEMPLATE_IMPLEMENTATION

	/**
	 * SupervisedPointerPolicy: policy used for supervised_ptr
	 */

	class CHAOS_API SupervisedPointerPolicy
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
			IntrusiveAddSupervisedReference(in_target);
			return in_target;
		}

		/** removing a reference */
		template<typename T>
		static T* SubReference(pointer_type<T>* in_target)
		{
			assert(in_target != nullptr);
			IntrusiveSubSupervisedReference(in_target);
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
	* checking whether a data is some kind of supervised pointer
	*/

	template<typename T>
	struct is_supervised_ptr
	{
		static constexpr bool value = false;
	};

	template<typename T>
	struct is_supervised_ptr<supervised_ptr<T>>
	{
		static constexpr bool value = true;
	};

	template<typename T>
	constexpr bool is_supervised_ptr_v = is_supervised_ptr<T>::value;

#endif

}; // namespace chaos

