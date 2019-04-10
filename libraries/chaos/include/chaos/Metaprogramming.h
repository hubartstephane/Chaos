#pragma once

#include <chaos/StandardHeaders.h>
#include <chaos/ReferencedObject.h>
#include <chaos/EmptyClass.h>

namespace chaos
{

	// create 2 metaclasses
	//
	//   - has_TRAITNAME<...> => boost::mpl::bool_
	//   - get_TRAITNAME<...> => the TRAIT if it exists
	//                           EmptyClass elsewhere

#define CHAOS_GENERATE_HAS_TRAIT(name)\
BOOST_MPL_HAS_XXX_TRAIT_DEF(name)\
template<typename T, typename B = typename has_##name<T>::type>\
class get_##name;\
template<typename T>\
class get_##name<T, boost::mpl::true_> : public boost::mpl::identity<typename T::name>\
{};\
template<typename T>\
class get_##name<T, boost::mpl::false_> : public boost::mpl::identity<chaos::EmptyClass>\
{};

	// create some meta class : has_function_NAME<A> => boost::mpl::bool_
	// XXX : it just test whether the T parameter has a member of given name, not (yet) if this is a function
#define CHAOS_GENERATE_HAS_FUNCTION_METACLASS(function_name)\
namespace details\
{\
char has_function_helper_##function_name(...);\
template<typename T>\
auto has_function_helper_##function_name(T const & t) -> decltype(&T::function_name);\
};\
template<typename T>\
using has_function_##function_name = boost::mpl::bool_<\
	sizeof(details::has_function_helper_##function_name(chaos::meta::GenerateFakeInstance<T>())) != 1\
>;

	// SFINAE
	// ------
	// XXX : here is an example of what kind of code, the preceding macro can generate
	//  
	//
	// char has_function_helper_XXXXXXXXX(...);
	//
	// template<typename T>
	// auto has_function_helper_XXXXXXXXX(T const & t) -> decltype(&T::XXXXXXXXX);
	//
	// template<typename T>
	// using has_function_XXXXXXXXX = boost::mpl::bool_<
	//	 sizeof(has_function_helper_XXXXXXXXX(chaos::meta::GenerateFakeInstance<T>())) != 1
	// >;
	//
	// XXX : boost can generate some 'similar' code with : BOOST_TTI_HAS_MEMBER_FUNCTION(XXXXXXX);
	//       the difference is that at usage, we have to ask for a precise function signature
	//
	//  has_member_function_XXXXXXXX<T, int (T::*)(float)>;    <-- function signature required (even return type)

	// Meta 
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
		static T * get_raw_pointer(shared_ptr<T> & src)
		{
			return src.get();
		}
		/** meta function to get a raw pointer from an input */
		template<typename T>
		static T const * get_raw_pointer(shared_ptr<T> const & src)
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

		/** a fake function (not implemented) that pretends to return a reference to an instance of T (does not deserve to be called) */
		template<typename T>
		T & GenerateFakeInstance();
	};

}; // namespace chaos
