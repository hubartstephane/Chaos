#pragma once

#include <chaos/StandardHeaders.h>

namespace chaos
{

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
		type * Allocate(size_t count){ assert(count > 0); return new type[count]; }
		/** a method to free a buffer of object */
		void   Free(type * ptr){ assert(ptr != nullptr); delete [] ptr; }
	};

}; // namespace chaos
