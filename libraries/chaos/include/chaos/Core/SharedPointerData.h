namespace chaos
{
#ifdef CHAOS_FORWARD_DECLARATION

	template<typename T>
	class SharedPointerData;

	template<typename T>
	using shared_ptr = SmartPointer<T, SharedPointerData>;

	template<typename T>
	struct is_shared_ptr;

#elif !defined CHAOS_TEMPLATE_IMPLEMENTATION

	/**
	 * SharedPointerData: internal implementation used by shared_ptr
	 */

	template<typename T>
	class SharedPointerData
	{
	public:

		/** the type of object pointed */
		using type = T;

		/** default constructor */
		SharedPointerData() = default;

		/** constructor with initialization */
		SharedPointerData(type * in_target):
			target(in_target)
		{
			if (target != nullptr)
				IntrusiveAddSharedReference(target);
		}

		/** copy constructor */
		SharedPointerData(SharedPointerData const & src):
			SharedPointerData(src.Get())
		{
		}

		/** move constructor */
		SharedPointerData(SharedPointerData && src) noexcept :
			target(src.target)
		{
			src.target = nullptr;
		}

		/** destructor */
		~SharedPointerData()
		{
			type* old_target = std::exchange(target, nullptr);
			if (old_target != nullptr)
				IntrusiveSubSharedReference(old_target);
		}

		/** move operator */
		SharedPointerData& operator = (SharedPointerData&& src) noexcept
		{
			if (this != &src)
			{
				Swap(src);
				src.Set(nullptr);
			}
			return *this;
		}

		/** swapping */
		void Swap(SharedPointerData & src)
		{
			std::swap(target, src.target);
		}

		/** set the pointed object */
		void Set(type * in_target)
		{
			if (target != in_target)
			{
				type* old_target = target;

				target = in_target;
				if (target != nullptr)
					IntrusiveAddSharedReference(target);
				
				if (old_target != nullptr)
					IntrusiveSubSharedReference(old_target);
			}		
		}

		/** get the pointed object */
		type * Get() const
		{
			return target;
		}

	protected:

		/** the object pointed */
		T * target = nullptr;
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

#if !defined CHAOS_FORWARD_DECLARATION && !defined CHAOS_TEMPLATE_IMPLEMENTATION

namespace std
{
	template<typename T>
	void swap(chaos::SharedPointerData<T>& src1, chaos::SharedPointerData<T>& src2)
	{
		src1.Swap(src2);
	}

}; // namespace std

#endif

