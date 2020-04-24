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

































	// ====================================================================================================
	// CHAOS_GENERATE_HAS_CALLABLE_FUNCTION(TOTO) generates 2 functions
	//
	// the first is useable directly with parameters
	//
	//		bool constexpr a1 = has_callable_toto<A>();
	//		bool constexpr a2 = has_callable_toto<A>(3);
	//		bool constexpr a3 = has_callable_toto<A>(4.0f, " ");
	//
	// the second is useable with types
	//
	//		bool constexpr a1 = has_callable2_toto<A>();
	//		bool constexpr a2 = has_callable2_toto<A, int>();
	//		bool constexpr a3 = has_callable2_toto<A, float>();
	//
	// with a short cut for the 2nd form
	//
	//      bool constexpr b = has_callable2_toto_v<A, int>;
	//
	// XXX: both forms are working with 'const'
	//
	//		bool constexpr a1 = has_callable1_toto<const A>(3);
	//		bool constexpr a1 = has_callable2_toto<const A, int>();
	//
	// XXX: the types given may not be strictly equals, but an implicit conversion should exist
	//
	//		function(double) <=== you can give a float
	//		function(float)  <=== you cannot give a double
	//
	// ====================================================================================================


#define CHAOS_GENERATE_HAS_CALLABLE_FUNCTION(funcname)\
namespace details\
{\
	template<typename T, typename ...PARAMS>\
	auto constexpr has_callable_##funcname##_helper(T & t, PARAMS... params) -> decltype(t.funcname(params...)) *;\
	char constexpr has_callable_##funcname##_helper(...);\
}\
template<typename T, typename ...PARAMS>\
bool constexpr has_callable1_##funcname##(PARAMS... params)\
{\
	return sizeof(details::has_callable_##funcname##_helper(chaos::meta::FakeInstance<T>(), params...)) != 1;\
}\
template<typename T, typename ...PARAMS>\
constexpr bool has_callable2_##funcname##()\
{\
	return sizeof(details::has_callable_##funcname##_helper(chaos::meta::FakeInstance<T>(), chaos::meta::FakeInstance<PARAMS>()...)) != 1;\
}\
template<typename T, typename ...PARAMS>\
constexpr bool has_callable2_##funcname##_v = has_callable2_##funcname<T, PARAMS...>();

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
		constexpr T & FakeInstance();
	
	}; // namespace meta

}; // namespace chaos
