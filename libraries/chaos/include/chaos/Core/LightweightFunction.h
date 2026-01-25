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
	 * 
	 * It's important that copy constructor/operator do not keep a reference upon their LightweightFunction parameter
	 * (this parameter is on the stack and is being destroyed at the end of the constructor/operator)
	 * 
	 * the templated functions tend to absorb the copy constructor/operator
	 * that's why the copy constructor/operator are removed and the distinction is being made thanks to if-constexpr
	 * 
	 * GOOD
	 * ----
	 *                      pointer
	 * LightweightFunction ---------> Callable   (the callable lifetime must be greater than the LightweightFunction's
	 * 
	 * 
	 * BAD
	 * ---
	 * 
	 *                      pointer                        pointer
	 * LightweightFunction ---------> LightweightFunction ---------> Callable
	 *                                        ^
	 *                                        |
	 *                                 parameter on the stack
	 *                   (being destroyed at the end of construor/copy operator)
	 * 
	 */

	template<typename RETURN_TYPE, typename... PARAMS>
	class LightweightFunction<RETURN_TYPE(PARAMS...)> // specialization of template for functions
	{
	public:

		/** default constructor */
		LightweightFunction() = default;
		/** constructor with initializer */
		template<std::invocable<PARAMS...> CALLABLE>
		LightweightFunction(CALLABLE const& callable)
		{
			if constexpr (std::is_same_v<CALLABLE, LightweightFunction>)
			{
				process_function = callable.process_function;
				src = callable.src;
			}
			else
			{
				process_function = &ProcessConst<CALLABLE>;
				src = (void*)(&callable);
			}
		}
		/** constructor with initializer */
		template<std::invocable<PARAMS...> CALLABLE>
		LightweightFunction(CALLABLE& callable)
		{
			if constexpr (std::is_same_v<CALLABLE, LightweightFunction>)
			{
				process_function = callable.process_function;
				src = callable.src;
			}
			else
			{
				process_function = &Process<CALLABLE>;
				src = (void*)(&callable);
			}
		}

		/** assign a new real underlying function */
		template<std::invocable<PARAMS...> CALLABLE>
		LightweightFunction& operator = (CALLABLE const& callable)
		{
			if constexpr (std::is_same_v<CALLABLE, LightweightFunction>)
			{
				process_function = callable.process_function;
				src = callable.src;
			}
			else
			{
				process_function = &ProcessConst<CALLABLE>;
				src = (void*)(&callable);
			}
			return *this;
		}
		/** assign a new real underlying function */
		template<std::invocable<PARAMS...> CALLABLE>
		LightweightFunction& operator = (CALLABLE& callable)
		{
			if constexpr (std::is_same_v<CALLABLE, LightweightFunction>)
			{
				process_function = callable.process_function;
				src = callable.src;
			}
			else
			{
				process_function = &Process<CALLABLE>;
				src = (void*)(&callable);
			}
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

	protected:

		/** internal method that make casts and call the underlying code */
		template<std::invocable<PARAMS...> CALLABLE>
		static RETURN_TYPE Process(void* src, PARAMS... params)
		{
			auto callable = reinterpret_cast<CALLABLE*>(src);
			return (*callable)(std::forward<PARAMS>(params)...);
		}

		/** internal method that make casts and call the underlying code */
		template<std::invocable<PARAMS...> CALLABLE>
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