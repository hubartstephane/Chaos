namespace chaos
{
#ifdef CHAOS_FORWARD_DECLARATION

	template<typename T, template<typename> class POINTER_DATA>
	class SmartPointer;

#elif !defined CHAOS_TEMPLATE_IMPLEMENTATION

	/**
	 * SmartPointer: base implementation for some smart pointers (shared, weak ...)
	 */

	template<typename T, template<typename> class POINTER_DATA>
	class SmartPointer
	{

	public:

		/** the type of object pointed */
		using type = T;

		/** default constructor */
		SmartPointer() = default;
		/** constructor with capturing the object */
		SmartPointer(type* in_target):
			pointer_data(in_target)
		{
		}
		/** copy constructor */
		SmartPointer(SmartPointer<T, POINTER_DATA> const& src) :
			pointer_data(src.pointer_data)
		{
		}
		/** move constructor */
		SmartPointer(SmartPointer<T, POINTER_DATA> && src) noexcept : // XXX : the noexcept is required to have move semantic used during vector resized
			pointer_data(std::move(src.pointer_data))
		{
		}

		/** copy/cast constructor */
		template<typename T2, template<typename> class POINTER_DATA2>
		SmartPointer(SmartPointer<T2, POINTER_DATA2> const& src) :
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
					pointer_data.Set(casted_src);
		}

		/** copy */
		SmartPointer& operator = (type* src)
		{
			pointer_data.Set(src);
			return *this;
		}
		/** copy */
		SmartPointer& operator = (SmartPointer<T, POINTER_DATA> const& src)
		{
			pointer_data.Set(src.get());
			return *this;
		}
		/** move */
		SmartPointer& operator = (SmartPointer<T, POINTER_DATA>&& src) noexcept
		{
			pointer_data = std::move(src.pointer_data);
			return *this;
		}

		/** copy */
		template<typename T2, template<typename> class POINTER_DATA2>
		SmartPointer& operator = (SmartPointer<T2, POINTER_DATA2> const& src)
		{
			static_assert(std::is_base_of_v<T, T2>);
			pointer_data.Set(src.get());
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
			return pointer_data.Get();
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
			std::swap(pointer_data, other.pointer_data); // this works for shared_ptr AND weak_ptr
		}

	protected:

		/** the object that manage the pointer */
		POINTER_DATA<T> pointer_data;
	};

	/**
	* SmartPointer / raw pointer comparaisons
	*/
	
	template<typename T, template<typename> class POINTER_DATA, typename U>
	bool operator == (SmartPointer<T, POINTER_DATA> const& src1, U* src2)
	{
		return (src1.get() == src2);
	}

	template<typename T, template<typename> class POINTER_DATA, typename U>
	bool operator == (U* src1, SmartPointer<T, POINTER_DATA> const& src2)
	{
		return (src1 == src2.get());
	}

	/**
	* SmartPointer / nullptr comparaisons
	*/

	template<typename T, template<typename> class POINTER_DATA>
	bool operator == (SmartPointer<T, POINTER_DATA> const& src1, nullptr_t src2)
	{
		return (src1.get() == src2);
	}

	template<typename T, template<typename> class POINTER_DATA>
	bool operator == (nullptr_t src1, SmartPointer<T, POINTER_DATA> const& src2)
	{
		return (src1 == src2.get());
	}

	/**
	* SmartPointer / SmartPointer comparaisons
	*/

	template<typename T, template<typename> class POINTER_DATA, typename U, template<typename> class POINTER_DATA2>
	bool operator == (SmartPointer<T, POINTER_DATA> const& src1, SmartPointer<U, POINTER_DATA2> const& src2)
	{
		return (src1.get() == src2.get());
	}

	/**
	* Swap
	*/

	template<typename T, template<typename> class POINTER_DATA>
	void Swap(SmartPointer<T, POINTER_DATA>& src1, SmartPointer<T, POINTER_DATA>& src2)
	{
		src1.swap(src2);
	}

#endif

}; // namespace chaos

#if !defined CHAOS_FORWARD_DECLARATION && !defined CHAOS_TEMPLATE_IMPLEMENTATION

namespace std
{
	template<typename T, template<typename> class POINTER_DATA>
	void swap(chaos::SmartPointer<T, POINTER_DATA>& src1, chaos::SmartPointer<T, POINTER_DATA>& src2)
	{
		src1.Swap(src2);
	}

}; // namespace std

#endif
