namespace chaos
{
#ifdef CHAOS_FORWARD_DECLARATION

	template<typename T, bool checked = false>
	class AutoConstCastable;

	template<typename T, bool checked = false>
	class AutoCastable;

#elif !defined CHAOS_TEMPLATE_IMPLEMENTATION

	/** AutoConstCastable : an object that provide dynamic_cast on demand */
	template<typename T, bool checked>
	class /*CHAOS_API*/ AutoConstCastable
	{
	public:

		/** the constructors */
		AutoConstCastable() = default;
		AutoConstCastable(AutoConstCastable const& src) = default;
		AutoConstCastable(T const* in_ptr) : ptr(in_ptr) {}

		/** the conversion operator */
		template<typename U>
		operator U const* () const
		{
			if constexpr (std::is_base_of_v<U, T>)
				return ptr;
			if constexpr (!std::is_polymorphic_v<T>)
			{
				assert(!checked);
				return nullptr;
			}
			else
			{
				auto result = dynamic_cast<const U*>(ptr);
				if constexpr (checked)
					assert(result != nullptr);
				return result;
			}
		}

		/** indirection operator */
		T const* operator -> () const { return ptr; }
		/** deferencing operator */
		T const& operator * () const { return *ptr; }
		/** checking whether this is valid */
		bool operator == (nullptr_t) const { return (ptr == nullptr); }
		/** checking whether this is valid */
		bool operator != (nullptr_t) const { return (ptr != nullptr); }
		/** bool operator (to be used in if statement */
		operator bool() const { return (ptr != nullptr); }

	protected:

		/** the pointer */
		T const* ptr = nullptr;
	};

	/** AutoCastable : an object that provide dynamic_cast on demand */
	template<typename T, bool checked>
	class /*CHAOS_API*/ AutoCastable
	{
	public:

		/** the constructors */
		AutoCastable() = default;
		AutoCastable(AutoCastable const& src) = default;
		AutoCastable(T* in_ptr) : ptr(in_ptr) {}

		/** the const conversion operator */
		operator AutoConstCastable<T>() const
		{
			return AutoConstCastable<T>(ptr);
		}
		/** the conversion operator */
		template<typename U>
		operator U* () const
		{
			if constexpr (std::is_base_of_v<U, T>)
				return ptr;
			if constexpr (!std::is_polymorphic_v<T>)
			{
				assert(!checked);
				return nullptr;
			}
			else
			{
				auto result = dynamic_cast<U*>(ptr);
				if constexpr (checked)
					assert(result != nullptr);
				return result;
			}
		}
		/** indirection operator */
		T* operator -> () const { return ptr; }
		/** deferencing operator */
		T& operator * () const { return *ptr; }
		/** checking whether this is valid */
		bool operator == (nullptr_t) const { return (ptr == nullptr); }
		/** checking whether this is valid */
		bool operator != (nullptr_t) const { return (ptr != nullptr); }
		/** bool operator (to be used in if statement */
		operator bool() const { return (ptr != nullptr); }

	protected:

		/** the pointer */
		T* ptr = nullptr;
	};

	/** create a delayed dynamic_cast<> */
	template<typename T>
	/*CHAOS_API*/ AutoCastable<T> auto_cast(AutoCastable<T> const& src) { return src; }
	/** create a delayed dynamic_cast<> */
	template<typename T>
	/*CHAOS_API*/ AutoConstCastable<T> auto_cast(AutoConstCastable<T> const& src) { return src; }
	/** create a delayed dynamic_cast<> */
	template<typename T>
	/*CHAOS_API*/ AutoCastable<T> auto_cast(T* ptr) { return AutoCastable<T>(ptr); }
	/** create a delayed dynamic_cast<> */
	template<typename T>
	/*CHAOS_API*/ AutoConstCastable<T> auto_cast(T const* ptr) { return AutoConstCastable<T>(ptr); }

	/** create a delayed dynamic_cast<> */
	template<typename T>
	/*CHAOS_API*/ AutoCastable<T, true> auto_cast_checked(AutoCastable<T> const& src) { return src; }
	/** create a delayed dynamic_cast<> */
	template<typename T>
	/*CHAOS_API*/ AutoConstCastable<T, true> auto_cast_checked(AutoConstCastable<T> const& src) { return src; }
	/** create a delayed dynamic_cast<> */
	template<typename T>
	/*CHAOS_API*/ AutoCastable<T, true> auto_cast_checked(T* ptr) { return AutoCastable<T, true>(ptr); }
	/** create a delayed dynamic_cast<> */
	template<typename T>
	/*CHAOS_API*/ AutoConstCastable<T, true> auto_cast_checked(T const* ptr) { return AutoConstCastable<T, true>(ptr); }

#endif

}; // namespace chaos