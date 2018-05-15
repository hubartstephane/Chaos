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
			// get a pointer on 'This' function and transform it into a uintptr_t
			// (remove const so this is not exactly a pointer on 'This' function)
			return (uintptr_t)FuntionPtrToVoidPtr(
				&GetClassID<boost::remove_const<T>::type>
			);
		};

		/** a function that helps converting a pointer of function into a void pointer */
		template<typename FUNC_PTR>
		void * FuntionPtrToVoidPtr(FUNC_PTR func_ptr)
		{
			union
			{
				FUNC_PTR    func_ptr;
				void      * ptr;
			} result;

			result.func_ptr = func_ptr;
			return result.ptr;
		}

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
