namespace chaos
{
#ifdef CHAOS_FORWARD_DECLARATION

	template<typename T, typename POLICY>
	class SmartPointer;

#elif !defined CHAOS_TEMPLATE_IMPLEMENTATION

	/**
	 * SmartPointer: base implementation for some smart pointers (shared, weak ...)
	 */

	template<typename T, typename POLICY>
	class SmartPointer
	{

	public:

		/** the type of object pointed */
		using type = T;

		/** default constructor */
		SmartPointer() = default;
		/** constructor with capturing the object */
		SmartPointer(type* in_target)
		{
			if (in_target != nullptr)
				target = POLICY::AddReference(in_target);
		}
		/** copy constructor */
		SmartPointer(SmartPointer<T, POLICY> const& src) :
			SmartPointer(src.get())
		{
		}
		/** move constructor */
		SmartPointer(SmartPointer<T, POLICY>&& src) noexcept : // XXX : the noexcept is required to have move semantic used during vector resized
			target(src.target)
		{
			src.target = nullptr; // capture the reference
		}

		/** copy/cast constructor */
		template<typename T2, typename POLICY2>
		SmartPointer(SmartPointer<T2, POLICY2> const& src) :
			SmartPointer(src.get())
		{
			static_assert(std::is_base_of_v<T, T2>);
		}
		/** constructor with AutoCastable */
		template<typename U>
		SmartPointer(AutoCastable<U> const& src)
		{
			if (src != nullptr)
				if (T * casted_src = (T*)src)
					target = POLICY::AddReference(casted_src);
		}

		/** destructor */
		~SmartPointer()
		{
			if (target != nullptr)
				target = POLICY::SubReference(target);
		}

		/** copy */
		SmartPointer& operator = (type* src)
		{
			if (src != get())
				DoSetTarget(src);
			return *this;
		}
		/** copy */
		SmartPointer& operator = (SmartPointer<T, POLICY> const& src)
		{
			if (src.target != target) // no need to fully get() => small optimization for weak_ptr
				DoSetTarget(src.get());
			return *this;
		}
		/** move */
		SmartPointer& operator = (SmartPointer<T, POLICY>&& src) noexcept
		{
			if (src.target != target)
			{
				// XXX : add reference before destroying to be sure that by chain reaction we do not destroy the object we want to point first
				auto old_target = target;

				// 1 - capture new the reference
				target = src.target;
				src.target = nullptr;
				// 2 - remove old reference
				if (old_target != nullptr)
					POLICY::SubReference(old_target);
			}
			return *this;
		}

		/** copy */
		template<typename T2, typename POLICY2>
		SmartPointer& operator = (SmartPointer<T2, POLICY2> const& src)
		{
			static_assert(std::is_base_of_v<T, T2>);
			if (src.target != target) // no need to fully get() => small optimization for weak_ptr
				DoSetTarget(src.get());
			return *this;
		}

		/** constructor with AutoCastable */
		template<typename U>
		SmartPointer& operator = (AutoCastable<U> const& src)
		{
			return operator = ((type*)src);
		}

		/** getters */
		type* get() const
		{
			return POLICY::template Get<type>(target);
		}

		/** getters */
		type* operator ->() const
		{
			type* result = get();
			assert(result != nullptr);
			return result;
		}

		/** getters */
		type& operator * () const
		{
			type* result = get();
			assert(result != nullptr);
			return *result;
		}

		/** swap pointers */
		void Swap(SmartPointer& other)
		{
			std::swap(target, other.target); // this works for shared_ptr AND weak_ptr
		}

	protected:

		/** internal method to change the content of the pointer */
		void DoSetTarget(type* src)
		{
			// XXX : add reference before destroying to be sure that by chain reaction we do not destroy the object we want to point first
			auto old_target = target;

			// 1 - add reference
			if (src != nullptr)
				target = POLICY::AddReference(src);
			else
				target = nullptr;
			// 2 - remove reference
			if (old_target != nullptr)
				POLICY::SubReference(old_target);
		}

	protected:

		/** the object pointed (or a WeakPointerData structure) */
		typename POLICY::template pointer_type<type>* target = nullptr;
	};

	/**
	* SmartPointer / raw pointer comparaisons
	*/

	template<typename T, typename POLICY, typename U>
	bool operator == (SmartPointer<T, POLICY> const& src1, U* src2)
	{
		return (src1.get() == src2);
	}

	template<typename T, typename POLICY, typename U>
	bool operator == (U* src1, SmartPointer<T, POLICY> const& src2)
	{
		return (src1 == src2.get());
	}

	/**
	* SmartPointer / nullptr comparaisons
	*/

	template<typename T, typename POLICY>
	bool operator == (SmartPointer<T, POLICY> const& src1, nullptr_t src2)
	{
		return (src1.get() == src2);
	}

	template<typename T, typename POLICY>
	bool operator == (nullptr_t src1, SmartPointer<T, POLICY> const& src2)
	{
		return (src1 == src2.get());
	}

	/**
	* SmartPointer / SmartPointer comparaisons
	*/

	template<typename T, typename POLICY, typename U, typename POLICY2>
	bool operator == (SmartPointer<T, POLICY> const& src1, SmartPointer<U, POLICY2> const& src2)
	{
		return (src1.get() == src2.get());
	}

	/**
	* Swap
	*/

	template<typename T, typename POLICY>
	void Swap(SmartPointer<T, POLICY>& src1, SmartPointer<T, POLICY>& src2)
	{
		src1.swap(src2);
	}

#endif

}; // namespace chaos

#if !defined CHAOS_FORWARD_DECLARATION && !defined CHAOS_TEMPLATE_IMPLEMENTATION

namespace std
{
	template<typename T, typename POLICY>
	void swap(chaos::SmartPointer<T, POLICY>& src1, chaos::SmartPointer<T, POLICY>& src2)
	{
		src1.Swap(src2);
	}

}; // namespace std

#endif
