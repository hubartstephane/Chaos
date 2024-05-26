namespace chaos
{
#ifdef CHAOS_FORWARD_DECLARATION

	template<typename>
	class LightweightFunction;

#elif !defined CHAOS_TEMPLATE_IMPLEMENTATION

	/**
	 * LightweightFunction: 
	 *    A class that deserves to be used as argument in method instead of std::function.
	 *    There is no copy, so any underlying object should have a longer lifetime.
	 *    Not deserved to be stored for later
	 */

	template<typename RETURN_TYPE, typename... PARAMS>
	class LightweightFunction<RETURN_TYPE(PARAMS...)> // specialization of template for functions
	{
	public:

		/** default constructor */
		LightweightFunction() = default;
		/** copy constructor */
		LightweightFunction(LightweightFunction const&) = default;
		/** move constructor */
		LightweightFunction(LightweightFunction&&) = default;
		/** constructor with initializer */
		template<typename CALLABLE>
		LightweightFunction(CALLABLE const& callable)
		{
			process_function = &ProcessConst<CALLABLE>;
			src = (void*)(&callable);
		}
		/** constructor with initializer */
		template<typename CALLABLE>
		LightweightFunction(CALLABLE& callable)
		{
			process_function = &Process<CALLABLE>;
			src = (void*)(&callable);
		}

		/** assignation operator */
		LightweightFunction& operator = (LightweightFunction const& src) = default;
		/** move operator */
		LightweightFunction& operator = (LightweightFunction&& src) = default;

		/** assign a new real underlying function */
		template<typename CALLABLE>
		LightweightFunction& operator = (CALLABLE const& callable)
		{
			process_function = &ProcessConst<CALLABLE>;
			src = (void*)(&callable);
			return *this;
		}
		/** assign a new real underlying function */
		template<typename CALLABLE>
		LightweightFunction& operator = (CALLABLE& callable)
		{
			process_function = &Process<CALLABLE>;
			src = (void*)(&callable);
			return *this;
		}

		/** call the functor */
		RETURN_TYPE operator () (PARAMS... params) const
		{
			assert(IsValid());
			return process_function(src, std::forward<PARAMS>(params)...);
		}

		/** check whether the object is valid */
		bool IsValid() const
		{
			return (process_function != nullptr);
		}

		/** check whether the object is valid */
		operator bool() const { return IsValid(); }

	protected:

		/** internal method that make casts and call the underlying code */
		template<typename CALLABLE>
		static RETURN_TYPE Process(void* src, PARAMS... params)
		{
			auto callable = reinterpret_cast<CALLABLE*>(src);
			return (*callable)(std::forward<PARAMS>(params)...);
		}

		/** internal method that make casts and call the underlying code */
		template<typename CALLABLE>
		static RETURN_TYPE ProcessConst(void* src, PARAMS... params)
		{
			auto callable = reinterpret_cast<CALLABLE const*>(src);
			return (*callable)(std::forward<PARAMS>(params)...);
		}

	protected:

		/** intermediate function pointer to make casts and call underlying code */
		RETURN_TYPE(*process_function)(void*, PARAMS...) = nullptr;
		/** the real object upon which the final call will be made */
		void* src = nullptr;
	};

#endif

}; // namespace chaos