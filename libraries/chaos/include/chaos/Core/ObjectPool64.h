namespace chaos
{
#ifdef CHAOS_FORWARD_DECLARATION

	template<typename T, bool ENABLE_RECYCLING = true>
	class ObjectPool64;

#elif !defined CHAOS_TEMPLATE_IMPLEMENTATION

	template<typename T>
	concept RecycleMethodExists = requires(T * src)
	{
		{ src->Recycle() };
	};

	template<typename T>
	concept RecycleFunctionExists = requires(T * src)
	{
		{ Recycle(src) };
	};

	/**
	* ObjectPool64RecyclableBase: a base for ObjectPool64 depending whether objects can be recycled
	*/

	namespace details
	{
		template<bool USE_RECYCLABLE_FEATURE>
		class ObjectPool64RecyclableBase
		{};

		template<>
		class ObjectPool64RecyclableBase<true>
		{
		protected:

			/** a bitfield indicating which instances has been constructed once (must be recycled) */
			uint64_t constructed_instances = 0;
		};


	}; // namespace details

	/**
	* This is an allocator that contains up to 64 instances of an object
	**/
	template<typename T, bool ENABLE_RECYCLING>
	class ObjectPool64 : public details::ObjectPool64RecyclableBase<ENABLE_RECYCLING || RecycleMethodExists<T> || RecycleFunctionExists<T>>
	{

	public:

		using type = T;

		static constexpr size_t pool_size = 64;

		static constexpr bool recycling_enabled = ENABLE_RECYCLING || RecycleMethodExists<T> || RecycleFunctionExists<T>;

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
			uint64_t objects = used_instances;
			if constexpr (recycling_enabled)
				objects |= this->constructed_instances;

			BitTools::ForEachBitForward(objects, [this](uint64_t index)
			{
				FreeHelper(GetObjectPtr(index), true);
			});
		}

		/** release an object inside the pool for further usage */
		void Free(type* object)
		{
			assert(object != nullptr);
			assert(IsObjectInsidePool(object));
			assert(IsAllocatedObject(object));

			FreeHelper(object, false);
		}

		/** allocate a new object from pool */
		template<typename ...PARAMS>
		type* Allocate(PARAMS && ...params)
		{
			if (!HasAvailableInstanceLeft())
				return nullptr;

			auto ReserveObjectForConstruction = [this](uint64_t objects_mask) -> type *
			{
				if (objects_mask == 0)
					return nullptr;

				uint64_t index = BitTools::bsr(objects_mask);

				used_instances = BitTools::SetBit(used_instances, index, true);
				if constexpr (recycling_enabled)
					this->constructed_instances = BitTools::SetBit(this->constructed_instances, index, true);

				return GetObjectPtr(index); // at this point, the return value might just a pointer on junk memory
			};

			// try in recycle list first
			if constexpr (recycling_enabled)
			{
				constexpr bool initialize_method_exists = requires(T * src, PARAMS && ...params)
				{
					src->Initialize(std::forward<PARAMS>(params)...);
				};

				constexpr bool initialize_function_exists = requires(T * src, PARAMS && ...params)
				{
					Initialize(src, std::forward<PARAMS>(params)...);
				};

				if constexpr (initialize_method_exists || initialize_function_exists)
				{
					// TRY 1: we have some elements we can recyle
					uint64_t existing_free_instance = ~used_instances & this->constructed_instances;
					if (type* result = ReserveObjectForConstruction(existing_free_instance))
					{
						if constexpr (initialize_method_exists) // method first (higher priority if both exists)
							result->Initialize(std::forward<PARAMS>(params)...);
						else if constexpr (initialize_function_exists)
							Initialize(result, std::forward<PARAMS>(params)...);

						return result;
					}
				}

				// TRY 2: we have some free room in the buffer with junk data inside: call constructor
				uint64_t non_existing_free_instance = ~used_instances & ~this->constructed_instances;
				if (type* result = ReserveObjectForConstruction(non_existing_free_instance))
				{
					new (result) type(std::forward<PARAMS>(params)...);
					return result;
				}

				// TRY 3: recycling is enabled but the arguments for this function don't match any Initialize(...) implementation
				//        free some existing but unused element first
				if constexpr (!initialize_method_exists && !initialize_function_exists)
				{
					uint64_t existing_free_instance = ~used_instances & this->constructed_instances;
					if (existing_free_instance != 0)
					{
						uint64_t index = BitTools::bsr(existing_free_instance);
						FreeHelper(GetObjectPtr(index), true); // let fallback do the rest
					}
				}
			}

			// fallback
			if (type* result = ReserveObjectForConstruction(~used_instances))
			{
				new (result) type(std::forward<PARAMS>(params)...);
				return result;
			}
			return nullptr;
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
			return GetReservedCount() != pool_size;
		}

		/** gets the number of reserved object */
		size_t GetReservedCount() const
		{
			return BitTools::popcount(used_instances);
		}

		/** iterator over all objects (const version) */
		template<typename FUNC>
		decltype(auto) ForEachObject(FUNC const& func) const
		{
			return chaos::BitTools::ForEachBitForward(used_instances, [this, &func](uint64_t index)
			{
				return func(GetObjectPtr(index));
			});
		}

		/** iterator over all objects (non const version) */
		template<typename FUNC>
		decltype(auto) ForEachObject(FUNC const& func)
		{
			return chaos::BitTools::ForEachBitForward(used_instances, [this, &func](uint64_t index)
			{
				return func(GetObjectPtr(index));
			});
		}

	protected:

		/** release an object inside the pool for further usage */
		void FreeHelper(type* object, bool final_destruction)
		{
			assert(object != nullptr);
			assert(IsObjectInsidePool(object));
			assert(IsValidObjectInstance(object));

			if (object != nullptr)
			{
				// update the available flag
				uint64_t index = GetObjectIndex(object);

				used_instances = BitTools::SetBit(used_instances, index, false);

				// try recyle first
				if constexpr (recycling_enabled)
					if (!final_destruction)
						if (BitTools::GetBit(this->constructed_instances, index))
							if (RecycleObjectHelper(object))
								return;

				// usual destruction elsewhere
				if constexpr (recycling_enabled)
					this->constructed_instances = BitTools::SetBit(this->constructed_instances, index, false);
				object->~type();
			}
		}

		/** Recycle the object */
		bool RecycleObjectHelper(type * object) const
		{
			if constexpr (RecycleMethodExists<T>) // method first (higher priority if both exists)
			{
				object->Recycle();
				return true;
			}
			else if constexpr (RecycleFunctionExists<T>)
			{
				Recycle(object);
				return true;
			}
			return false;
		}

		/** Check whether there is a valid object instance at given index */
		bool IsValidObjectInstance(type const * object) const
		{
			if (IsAllocatedObject(object))
				return true;
			if constexpr (recycling_enabled)
			{
				uint64_t index = GetObjectIndex(object);
				if (this->constructed_instances & (uint64_t(1) << index))
					return true;
			}
			return false;		
		}

		/** Check whether the object is allocated */
		bool IsAllocatedObject(type const * object) const
		{
			uint64_t index = GetObjectIndex(object);
			if (used_instances & (uint64_t(1) << index))
				return true;
			return false;
		}

		/** gets the index of an object inside the pool */
		uint64_t GetObjectIndex(type const* object) const
		{
			assert(object != nullptr);
			return uint64_t(object - GetObjectPtr(0));
		}

		/** gets the address of an object inside the pool */
		T const* GetObjectPtr(uint64_t index) const
		{
			assert(index < pool_size);
			return ((type*)data) + index;
		}

		/** gets the address of an object inside the pool */
		T* GetObjectPtr(uint64_t index)
		{
			assert(index < pool_size);
			return ((type*)data) + index;
		}

	protected:

		/** a bitfield indicating which instances are in use */
		uint64_t used_instances = 0;
		/** the block of data where instanced are being used */
		alignas(type) char data[pool_size * sizeof(type)];
	};

#endif
}; // namespace chaos

