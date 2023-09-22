namespace chaos
{
#ifdef CHAOS_FORWARD_DECLARATION

	template<typename T>
	class ObjectPool64;

#elif !defined CHAOS_TEMPLATE_IMPLEMENTATION

	/**
	* This is an allocator that contains up to 64 instances of an object
	**/
	template<typename T>
	class ObjectPool64
	{

	public:

		using type = T;

		static constexpr size_t pool_size = 64;

		/** constructor */
		ObjectPool64() = default;
		/** no copy constructor */
		ObjectPool64(ObjectPool64 const& src) = delete;
		/** no copy operator */
		ObjectPool64& operator = (ObjectPool64 const& src) = delete;

		/** destructor */
		~ObjectPool64()
		{
			// destroy all objects
			BitTools::ForEachBitForward(used_instanced, [this](int64_t index)
				{
					Free(GetObjectPtr(index));
				});
		}

		/** release an object inside the pool for further usage */
		void Free(type* object)
		{
			if (object != nullptr)
			{
				assert(IsObjectInsidePool(object));
				// update the available flag
				int64_t index = GetObjectIndex(object);
				assert((used_instanced >> index) & 1); // ensure the object was not already freed
				used_instanced = BitTools::SetBit(used_instanced, index, false);
				--reserved_count;
				// manually call the destructor
				object->~type();
			}
		}

		/** allocate a new object from pool */
		template<typename ...PARAMS>
		type* Allocate(PARAMS ...params)
		{
			if (!HasAvailableInstanceLeft())
				return nullptr;
			// update the available flag
			int64_t index = BitTools::bsr(~used_instanced);
			used_instanced = BitTools::SetBit(used_instanced, index, true);
			++reserved_count;
			// manually call constructor
			type* result = GetObjectPtr(index);
			new (result) type(std::forward<PARAMS>(params)...);
			return result;
		}

		/** check whether an object is inside the pool */
		bool IsObjectInsidePool(type const* object) const
		{
			assert(object != nullptr);
			return (object >= GetObjectPtr(0)) && (object <= GetObjectPtr(pool_size - 1));
		}

		/** returns true whether all instanced have allready been allocated */
		bool HasAvailableInstanceLeft() const
		{
			return (used_instanced != int64_t(-1));
		}

		/** gets the number of reserved object */
		size_t GetReservedCount() const
		{
			return reserved_count;
		}

	protected:

		/** gets the index of an object inside the pool */
		int64_t GetObjectIndex(type const* object) const
		{
			assert(object != nullptr);
			return int64_t(object - GetObjectPtr(0));
		}

		/** gets the address of an object inside the pool */
		T* GetObjectPtr(int64_t index) const
		{
			assert(index >= 0);
			assert(index < pool_size);
			return ((type*)data) + index;
		}

	protected:

		/** a bitfield indicating with instances are in use */
		int64_t used_instanced = 0;
		/** number of reserved object */
		size_t reserved_count = 0;
		/** the block of data where instanced are being used */
		alignas(8) char data[pool_size * sizeof(T)];
	};

#endif
}; // namespace chaos

