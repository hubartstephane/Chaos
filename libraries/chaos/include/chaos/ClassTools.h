#pragma once

#include <chaos/StandardHeaders.h>
#include <chaos/AllocatorTools.h>

namespace chaos
{
	namespace ClassTools
	{
		/** GetClassID : returns a unique ID given a class */
		template<typename T>
		uintptr_t GetClassID()
		{
			static T * fake_pointer = nullptr;
			return (uintptr_t)&fake_pointer;
		};

		/** Aligned16 : base class for children that want to be 16 aligned */
		class Aligned16
		{
		public:

			/** the new operator */
			void * operator new (size_t size){ return AllocatorTools::Aligned16Alloc(size);}
			/** the delete operator */
			void operator delete(void * ptr){ AllocatorTools::Aligned16Free(ptr);}
		};
	
	}; // namespace ClassTools

}; // namespace chaos
