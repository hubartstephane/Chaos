namespace chaos
{
#ifdef CHAOS_FORWARD_DECLARATION

	class WeakPointerControlBlock;

	template<typename T>
	class WeakPointerData;

	template<typename T>
	using weak_ptr = SmartPointer<T, WeakPointerData>;

	template<typename T>
	struct is_weak_ptr;

#elif !defined CHAOS_TEMPLATE_IMPLEMENTATION

	//   weak_ptr -----------> target
	//       |                    |
	//       |                    |
	//       |                    v
	//       +-----> WeakPointerControlBlock
	//                            |
	//                            +---- is_target_valid
	//                            |
	//                            +---- weak_ref_count
	//
	//    When using a weak_ptr on an object, the first time it will cause a memory allocation: WeakPointerControlBlock
	//
	//    Each time the weak_ptr tryes to access the object, it has to check in the WeakPointerControlBlock whether the target is
	//    still valid. It may destroy the WeakPointerControlBlock if necessary.
	//    weak_ptr is a bit slower

	/**
	 * WeakPointerControlBlock : an utility structure used to handle weak pointers
	 */

	class CHAOS_API WeakPointerControlBlock
	{
	public:

		/** constructor */
		WeakPointerControlBlock():
			is_target_valid(true),
			weak_ref_count(0)
		{
		}

		/** called whenever the object has being destroyed */
		void OnTargetDestroyed()
		{
			if (weak_ref_count == 0)
				delete(this);
			else
				is_target_valid = false;
		}
		/** new weak_ptr reference */
		void AddReference()
		{			
			++weak_ref_count;
		}

		/** called whenever a weak_ptr does not reference the control block anymore */
		void SubReference()
		{
			if (--weak_ref_count == 0)
				if (!is_target_valid)
					delete(this);
		}

	public:

		/** whether the object is still alive */
		std::atomic<bool> is_target_valid;
		/** count weak reference */
		std::atomic<int> weak_ref_count;
	};

	/**
	 * WeakPointerData: internal implementation used by weak_ptr
	 */

	template<typename T>
	class WeakPointerData
	{
	public:

		/** the type of object pointed */
		using type = T;

		/** default constructor */
		WeakPointerData() = default;

		/** constructor with initialization */
		WeakPointerData(type* in_target)
		{
			Set(in_target);
		}

		/** copy constructor */
		WeakPointerData(WeakPointerData const& src) :
			WeakPointerData(src.Get())
		{
		}

		/** move constructor */
		WeakPointerData(WeakPointerData&& src) noexcept :
			target(src.target),
			control_block(src.control_block)
		{
			src.target = nullptr;
			src.control_block = nullptr;
		}

		/** destructor */
		~WeakPointerData()
		{
			Set(nullptr);
		}

		/** move operator */
		WeakPointerData& operator = (WeakPointerData && src) noexcept
		{
			Swap(src);
			src.Set(nullptr);
			return *this;
		}

		/** swapping */
		void Swap(WeakPointerData& src)
		{
			std::swap(target, src.target);
			std::swap(control_block, src.control_block);
		}

		/** set the pointed object */
		void Set(type* in_target)
		{
			type * current_target = Get(); // this will, clean data if necessary

			if (current_target != in_target)
			{
				if (current_target)
				{
					control_block->SubReference();
					control_block = nullptr;
				}
				target = in_target;
				if (target != nullptr)
				{
					WeakPointerControlBlock* & target_control_block = target->weak_ptr_control_block;
					if (target_control_block == nullptr)
						target_control_block = new WeakPointerControlBlock;
					target_control_block->AddReference();
					control_block = target_control_block;
				}
			}
		}

		/** get the pointed object */
		type* Get() const
		{
			if (target != nullptr)
			{
				assert(control_block != nullptr);
				if (control_block->is_target_valid)
				{
					return target;
				}
				else
				{
					control_block->SubReference();
					target = nullptr;
					control_block = nullptr;
				}
			}
			return nullptr;
		}

	protected:

		/** the pointed object */
		mutable type * target = nullptr;
		/** the control block */
		mutable WeakPointerControlBlock * control_block = nullptr;
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

#if !defined CHAOS_FORWARD_DECLARATION && !defined CHAOS_TEMPLATE_IMPLEMENTATION

namespace std
{
	template<typename T>
	void swap(chaos::WeakPointerData<T>& src1, chaos::WeakPointerData<T>& src2)
	{
		src1.Swap(src2);
	}

}; // namespace std

#endif