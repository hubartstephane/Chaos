#include "chaos/ChaosPCH.h"
#include "chaos/ChaosInternals.h"

namespace chaos
{
	namespace AllocatorTools
	{
		void* Aligned16Alloc(size_t size)
		{
			return _aligned_malloc(size, 16);
		}

		void Aligned16Free(void* p)
		{
			_aligned_free(p);
		}

	}; // namespace AllocatorTools

}; // namespace chaos
