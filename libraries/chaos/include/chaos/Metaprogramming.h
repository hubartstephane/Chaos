#pragma once

#include <chaos/StandardHeaders.h>

namespace chaos
{
	namespace meta
	{
		/** meta function to get a raw pointer from an input */
		template<typename T>
		static T * get_raw_pointer(T * src)
		{
			return src;
		}
		/** meta function to get a raw pointer from an input */
		template<typename T>
		static T const * get_raw_pointer(T const * src)
		{
			return src;
		}
		/** meta function to get a raw pointer from an input */
		template<typename T>
		static T * get_raw_pointer(T & src)
		{
			return &src;
		}
		/** meta function to get a raw pointer from an input */
		template<typename T>
		static T const * get_raw_pointer(T const & src)
		{
			return &src;
		}
		/** meta function to get a raw pointer from an input */
		template<typename T>
		static T * get_raw_pointer(boost::intrusive_ptr<T> & src)
		{
			return src.get();
		}
		/** meta function to get a raw pointer from an input */
		template<typename T>
		static T const * get_raw_pointer(boost::intrusive_ptr<T> const & src)
		{
			return src.get();
		}
		/** meta function to get a raw pointer from an input */
		template<typename T>
		static T * get_raw_pointer(std::unique_ptr<T> & src)
		{
			return src.get();
		}
		/** meta function to get a raw pointer from an input */
		template<typename T>
		static T const * get_raw_pointer(std::unique_ptr<T> const & src)
		{
			return src.get();
		}


		/** remove all pointer */
		template<typename T>
		struct remove_all_pointer : boost::mpl::eval_if<
			boost::is_pointer<T>,
			remove_all_pointer<typename boost::remove_pointer<T>::type>,
			boost::mpl::identity<T>
		> {};

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
