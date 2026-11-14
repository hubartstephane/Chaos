namespace chaos
{
#ifdef CHAOS_FORWARD_DECLARATION

	template<typename T, template<typename> class REFERENCE_COUNTER>
	class OwnerPointerDataBase;

#elif !defined CHAOS_TEMPLATE_IMPLEMENTATION

	/**
	 * OwnerPointerDataBase: a base class for shared_ptr & supervised_ptr that gives responsability of references to some REFERENCE_COUNTER base
	 */

	template<typename T, template<typename> class REFERENCE_COUNTER>
	class OwnerPointerDataBase : public REFERENCE_COUNTER<T>
	{
	public:

		/** the type of object pointed */
		using type = T;

		/** default constructor */
		OwnerPointerDataBase() = default;

		/** constructor with initialization */
		OwnerPointerDataBase(type * in_target):
			target(in_target)
		{
			if (target != nullptr)
				this->AddReferenceToTarget(target);
		}

		/** copy constructor */
		OwnerPointerDataBase(OwnerPointerDataBase const & src):
			OwnerPointerDataBase(src.Get())
		{
		}

		/** move constructor */
		OwnerPointerDataBase(OwnerPointerDataBase&& src) noexcept :
			target(src.target)
		{
			src.target = nullptr;
		}

		/** destructor */
		~OwnerPointerDataBase()
		{
			type* old_target = std::exchange(target, nullptr);
			if (old_target != nullptr)
				this->SubReferenceToTarget(old_target);
		}

		/** move operator */
		OwnerPointerDataBase& operator = (OwnerPointerDataBase&& src) noexcept
		{
			if (this != &src)
			{
				Swap(src);
				src.Set(nullptr);
			}
			return *this;
		}

		/** swapping */
		void Swap(OwnerPointerDataBase& src)
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
					this->AddReferenceToTarget(target);
				
				if (old_target != nullptr)
					this->SubReferenceToTarget(old_target);
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

#endif

}; // namespace chaos

#if !defined CHAOS_FORWARD_DECLARATION && !defined CHAOS_TEMPLATE_IMPLEMENTATION

namespace std
{
	template<typename T, template<typename> class REFERENCE_COUNTER>
	void swap(chaos::OwnerPointerDataBase<T, REFERENCE_COUNTER>& src1, chaos::OwnerPointerDataBase<T, REFERENCE_COUNTER>& src2)
	{
		src1.Swap(src2);
	}

}; // namespace std

#endif

