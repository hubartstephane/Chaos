namespace chaos
{
#ifdef CHAOS_FORWARD_DECLARATION

	template<typename T>
	class SupervisedPointerReferenceCounter;

	template<typename T>
	using SupervisedPointerData = OwnerPointerDataBase<T, SupervisedPointerReferenceCounter>;

	template<typename T>
	using supervised_ptr = SmartPointer<T, SupervisedPointerData>;

	template<typename T>
	struct is_supervised_ptr;

#elif !defined CHAOS_TEMPLATE_IMPLEMENTATION

	/**
	 * SupervisedPointerReferenceCounter: a class responsible for adding/removing supervised counters
	 */

	template<typename T>
	class SupervisedPointerReferenceCounter
	{
	public:

		/** adding a reference to a target object */
		void AddReferenceToTarget(T * in_target)
		{
			IntrusiveAddSupervisedReference(in_target);
		}
		/** removing a reference to a target object */
		void SubReferenceToTarget(T* in_target)
		{
			IntrusiveSubSupervisedReference(in_target);
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

