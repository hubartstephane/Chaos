#pragma once

#include <chaos/StandardHeaders.h>

namespace chaos
{
	namespace meta
	{
		/** remove all pointer */
		template<typename T>
		struct remove_all_pointer : boost::mpl::eval_if<
			boost::is_pointer<T>,
			remove_all_pointer<typename boost::remove_pointer<T>::type>,
			boost::mpl::identity<T>
		>{};	

		/** add a pointer only if it is not already a pointer */
		template<typename T>
		using add_uniq_pointer = boost::mpl::eval_if<
			boost::is_pointer<T>,
			boost::mpl::identity<T>,
      boost::add_pointer<T>
		>;	

		/** returns true_ whether a sequence has an element satisfying a predicat */
		template<typename SEQ, typename PRED>
		using has_satisfying_element = boost::mpl::not_<
			boost::is_same<
			typename boost::mpl::end<SEQ>::type,
			typename boost::mpl::find_if<SEQ, PRED>::type
			>
		>;
	};

}; // namespace chaos
