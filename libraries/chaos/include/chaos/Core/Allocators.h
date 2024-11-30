namespace chaos
{
#ifdef CHAOS_FORWARD_DECLARATION

	template<typename T> class ArrayAllocator;
	template<typename T> class StandardAllocator;

#elif !defined CHAOS_TEMPLATE_IMPLEMENTATION

	/**
	* a class for default allocation
	*/

	template<typename T>
	class StandardAllocator
	{
	public:

		/** the type of object to allocate */
		using type = T;

		/** release an object */
		void Free(type* object)
		{
			if (object != nullptr)
				delete (object);
		}

		/** allocate a new object */
		template<typename ...PARAMS>
		type* Allocate(PARAMS ...params)
		{
			return new type(std::forward<PARAMS>(params)...);
		}
	};

	/**
	* a default class for Allocation of Array
	*/

	template<typename T>
	class ArrayAllocator
	{
	public:

		/** the type of object to allocate */
		using type = T;

		/** a method to allocate a buffer of object */
		type* Allocate(size_t count) { assert(count > 0); return new type[count]; }
		/** a method to free a buffer of object */
		void   Free(type* ptr) { assert(ptr != nullptr); delete[] ptr; }
	};

#endif

}; // namespace chaos