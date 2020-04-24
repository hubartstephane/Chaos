#pragma once

#include <chaos/StandardHeaders.h>
#include <chaos/ReferencedObject.h>
#include <chaos/EmptyClass.h>

namespace chaos
{

	// creates 2 metaclasses
	//
	//   - has_TRAITNAME<...> => boost::mpl::bool_
	//   - get_TRAITNAME<...> => the TRAIT if it exists
	//                           EmptyClass elsewhere
    // create 1 CONSTEXP bool 
    //
    //   - has_TRAITNAME_t<...> => true/false

#define CHAOS_GENERATE_HAS_TRAIT(name)\
BOOST_MPL_HAS_XXX_TRAIT_DEF(name)\
template<typename T>\
auto constexpr has_##name##_v = has_##name<T>::value;\
template<typename T, typename B = typename has_##name<T>::type>\
class get_##name;\
template<typename T>\
class get_##name<T, boost::mpl::true_> : public boost::mpl::identity<typename T::name>\
{};\
template<typename T>\
class get_##name<T, boost::mpl::false_> : public boost::mpl::identity<chaos::EmptyClass>\
{};

    // creates 1 metaclass
    //
    //   - has_MEMBERNAME<...> => boost::mpl::bool_
    //
    // creates 1 CONSPEXP bool
    //
    //   - has_MEMBERNAME_v<...> => true/false
    
#define CHAOS_GENERATE_HAS_MEMBER(member_name)\
BOOST_DECLARE_HAS_MEMBER(has_##member_name, member_name);\
template<typename T>\
auto constexpr has_##member_name##_v = has_##member_name<T>::value;
          
	// creates 1 metaclass 
    //
    //  - has_FUNCTIONNAME<...> => boost::mpl::bool_
    //
    // creates 1 CONSTEXP bool
    //
    //  - has_FUNCTIONNAME_v<...> => true/false
    //
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
	sizeof(details::has_function_helper_##function_name(chaos::meta::FakeInstance<T>())) != 1\
>;\
template<typename T>\
auto constexpr has_function_##function_name##_v = has_function_##function_name<T>::value;


























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
	//
	// XXX : limitation !!!
	//
	//  this is not working if the class has a XXXXXXXXX TEMPLATE function
	//
















	// ====================================================================================================
	// CHAOS_GENERATE_HAS_CALLABLE_FUNCTION(TOTO) generates a function that returns 
	//
	//   bool constexpr b = has_callable_TOTO<A>(1, 2, 3)
	//
	// ====================================================================================================


#define CHAOS_GENERATE_HAS_CALLABLE_FUNCTION(funcname)\
namespace details\
{\
	template<typename T>\
	auto constexpr has_callable_##funcname##_helper_no_params(T & t) -> decltype(t.funcname()) *;\
	char constexpr has_callable_##funcname##_helper_no_params(...);\
\
	template<typename T, typename ...PARAMS>\
	auto constexpr has_callable_##funcname##_helper(T & t, PARAMS... params) -> decltype(t.funcname(params...)) *;\
	char constexpr has_callable_##funcname##_helper(...);\
}\
template<typename T, typename ...PARAMS>\
bool constexpr has_callable_##funcname##(PARAMS... params)\
{\
	return sizeof(details::has_callable_##funcname##_helper(chaos::meta::FakeInstance<T>(), params...)) != 1;\
}\
template<typename T>\
bool constexpr has_callable_##funcname##()\
{\
	return sizeof(details::has_callable_##funcname##_helper_no_params(chaos::meta::FakeInstance<T>())) != 1;\
}

	// ==================================================
	// Meta functions
	// ==================================================

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


		/** a fake function (not implemented) that pretends to return a reference to an instance of T (does not deserve to be called) */
		template<typename T>
		T & FakeInstance();
	
	}; // namespace meta

}; // namespace chaos
