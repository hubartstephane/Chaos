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

		/** if T has a member type, returns (recursively) the type of type. returns T elsewhere */
		template<typename T, typename HAS_TYPE = typename boost::mpl::aux::has_type<T>::type>
		struct get_type;
		/** specialisation get_type */
		template<typename T>
		struct get_type<T, boost::mpl::true_> : public get_type<typename T::type> {};
		/** specialisation get_type */
		template<typename T>
		struct get_type<T, boost::mpl::false_> : public boost::mpl::identity<T> {};


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
