#pragma once

#include <chaos/StandardHeaders.h>

namespace chaos
{
	namespace Metaprog
	{
		/** add a pointer only if it is not already a pointer */
		template<typename T>
		using add_uniq_pointer = boost::mpl::eval_if<
			boost::is_pointer<T>,
			boost::mpl::identity<T>,
      boost::add_pointer<T>
		>;	
	};

}; // namespace chaos
