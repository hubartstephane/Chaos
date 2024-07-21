namespace chaos
{
	namespace AllocatorTools
	{
#if !defined CHAOS_FORWARD_DECLARATION && !defined CHAOS_TEMPLATE_IMPLEMENTATION

		/** an aligned allocator */
		CHAOS_API void* Aligned16Alloc(size_t size);
		/** an aligned allocator */
		CHAOS_API void Aligned16Free(void* p);

		/** this function is used to have multiple object constructed on a continus chunk of memory */
		template<typename T1, typename T2>
		std::tuple<T1*, T2*> SingleAllocMultipleObjects(size_t count1, size_t count2)
		{
			std::tuple<T1*,T2*> result(nullptr, nullptr);

			size_t size1 = count1 * sizeof(T1);
			size_t size2 = count2 * sizeof(T2);

			size_t size = ((size1 + 15) & ~0x0F) + size2;

			// XXX :
			//  malloc(...) is aligned 8 on WIN32 and 16 on WIN64
			//  we want to force alignment 16, so use special alignment malloc functions
			//   - on GNU use              aligned_malloc(align, size),
			//   - here on window this is _aligned_malloc(size, align);
			//   => beware function name (underscore)
			//   => beware order of parameters
			//
			void * buffer = Aligned16Alloc(size);
			if (buffer != nullptr)
			{
				intptr_t first  = (intptr_t)buffer;
				intptr_t second = ((first + size1 + 15) & ~0x0F);

				assert(first  % 16 == 0);
				assert(second % 16 == 0);

				std::get<0>(result) = (T1*)first;
				std::get<1>(result) = (T2*)second;

				for (size_t i = 0 ; i < count1 ; ++i)
					new (std::get<0>(result) + i) T1();
				for (size_t i = 0 ; i < count2 ; ++i)
					new (std::get<1>(result) + i) T2();
			}
			return result;
		}

#endif

	}; // namespace AllocatorTools

}; // namespace chaos