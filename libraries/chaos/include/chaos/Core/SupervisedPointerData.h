namespace chaos
{
#ifdef CHAOS_FORWARD_DECLARATION

	template<typename T>
	class SupervisedPointerData;

	template<typename T>
	using supervised_ptr = SmartPointer<T, SupervisedPointerData>;

	template<typename T>
	struct is_supervised_ptr;

#elif !defined CHAOS_TEMPLATE_IMPLEMENTATION

	/**
	 * SupervisedPointerData: internal implementation used by supervised_ptr
	 */

	template<typename T>
	class SupervisedPointerData
	{
	public:

		/** the type of object pointed */
		using type = T;

		/** default constructor */
		SupervisedPointerData() = default;

		/** constructor with initialization */
		SupervisedPointerData(type* in_target) :
			target(in_target)
		{
			if (target != nullptr)
				IntrusiveAddSupervisedReference(target);
		}

		/** copy constructor */
		SupervisedPointerData(SupervisedPointerData const& src) :
			SupervisedPointerData(src.Get())
		{
		}

		/** move constructor */
		SupervisedPointerData(SupervisedPointerData&& src) noexcept :
			target(src.target)
		{
			src.target = nullptr;
		}

		/** destructor */
		~SupervisedPointerData()
		{
			type* old_target = std::exchange(target, nullptr);
			if (old_target != nullptr)
				IntrusiveSubSupervisedReference(old_target);
		}

		/** move operator */
		SupervisedPointerData& operator = (SupervisedPointerData&& src) noexcept
		{
			if (this != &src)
			{
				Swap(src);
				src.Set(nullptr);
			}
			return *this;
		}

		/** swapping */
		void Swap(SupervisedPointerData& src)
		{
			std::swap(target, src.target);
		}

		/** set the pointed object */
		void Set(type* in_target)
		{
			if (target != in_target)
			{
				type* old_target = target;

				target = in_target;
				if (target != nullptr)
					IntrusiveSubSupervisedReference(target);

				if (old_target != nullptr)
					IntrusiveAddSupervisedReference(old_target);
			}
		}

		/** get the pointed object */
		type* Get() const
		{
			return target;
		}

	protected:

		/** the object pointed */
		T* target = nullptr;
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

#if !defined CHAOS_FORWARD_DECLARATION && !defined CHAOS_TEMPLATE_IMPLEMENTATION

namespace std
{
	template<typename T>
	void swap(chaos::SupervisedPointerData<T>& src1, chaos::SupervisedPointerData<T>& src2)
	{
		src1.Swap(src2);
	}

}; // namespace std

#endif

