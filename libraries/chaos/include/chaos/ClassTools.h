#pragma once

#include <chaos/StandardHeaders.h>

namespace chaos
{
	namespace ClassTools
	{

		/**
		 * Aligned16 : base class for children that want to be 16 aligned
		 */
		class Aligned16
		{
		public:

			/** the new operator */
			void * operator new (size_t size){ return _aligned_malloc( 16, size );}
			/** the delete operator */
			void operator delete(void * ptr){ _aligned_free( ptr);}		
		};
	
	}; // namespace ClassTools

}; // namespace chaos
