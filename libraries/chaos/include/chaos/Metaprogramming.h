#pragma once

#include <chaos/StandardHeaders.h>
#include <chaos/Object.h>
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
	// CHAOS_GENERATE_CHECK_METHOD(toto) generates 
	//
	// - function returning BOOL whether the class 'T' (the very first template parameter) has a callable method called 'toto'
	//
	//		bool constexpr a1 = check_method_toto<T>();
	//		bool constexpr a2 = check_method_toto<T, int>();
	//		bool constexpr a3 = check_method_toto<T, float, char>();
	//
	// - there is a shortcut to have directly the BOOL value
	//
	//      bool constexpr b = check_method_toto_v<T, int>;
	//
	// XXX: the parameter T may be CONST or NON-CONST (this works)
	//
	//		bool constexpr a1 = check_method_toto<T, int>();
	//		bool constexpr a1 = check_method_toto<T const, int>();
	//
	// XXX: the types given may not be strictly equals, but an implicit conversion should exist
	//
	//		function(double) <=== you can give a float
	//		function(float)  <=== you cannot give a double
	//
	// The template produce a decltype(...) shortcut too
	//
	//		typeof_method_toto<T, int> ---> this give the type of the result of a call to T().toto(333);
	//
	// ====================================================================================================


#define CHAOS_GENERATE_CHECK_METHOD(funcname)\
namespace details\
{\
	template<typename T, typename ...PARAMS>\
	auto constexpr check_method_##funcname##_helper(T & t, PARAMS... params) -> decltype(t.funcname(params...)) *;\
	char constexpr check_method_##funcname##_helper(...);\
}\
template<typename T, typename ...PARAMS>\
constexpr bool check_method_##funcname()\
{\
	return sizeof(details::check_method_##funcname##_helper(chaos::meta::FakeInstance<T>(), chaos::meta::FakeInstance<PARAMS>()...)) != 1;\
}\
\
template<typename T, typename ...PARAMS>\
constexpr bool check_method_##funcname##_v = check_method_##funcname<T, PARAMS...>();\
\
template<typename T, typename ...PARAMS>\
using typeof_method_##funcname = decltype(chaos::meta::FakeInstance<T>().funcname(chaos::meta::FakeInstance<PARAMS>()...));\


	// ====================================================================================================
	// CHAOS_GENERATE_CHECK_FUNCTION(toto) generates 
	//
	// - function returning BOOL whether there is a function 'toto" that can be called with following parameters
	//
	//		bool constexpr a1 = check_function_toto<>();
	//		bool constexpr a2 = check_function_toto<int>();
	//		bool constexpr a3 = check_function_toto<float, char>();
	//
	// - there is a shortcut to have directly the BOOL value
	//
	//      bool constexpr b = check_function_toto_v<int>;
	//
	// XXX: the types given may not be strictly equals, but an implicit conversion should exist
	//
	//		function(double) <=== you can give a float
	//		function(float)  <=== you cannot give a double
	//
	// The template produce a decltype(...) shortcut too
	//
	//		typeof_function_toto<int> ---> this give the type of the result of a call to toto(333);
	//
	//
	// XXX : during implementation of thoses SFINAE code, i encoutered several issues
	//
	//	     I forced an additionnal template parameter T (and i give it an int), so there is no ambiguity between   f(no parameters)  & f(...)
	//
	//       typeof_function_toto<...> was not working every time if i was using directly decltype(...). So i used an intermediate meta function class
	//
	//       (the compiler was immediatly returning an error,   at decltype(toto())   because it didn't know about the function toto() yet
	//
	// ====================================================================================================

#define CHAOS_GENERATE_CHECK_FUNCTION(funcname)\
namespace details\
{\
	template<typename T, typename ...PARAMS>\
	auto constexpr check_function_##funcname##_helper(T t, PARAMS... params) -> decltype(funcname(params...)) *;\
	char constexpr check_function_##funcname##_helper(...);\
	template<typename ...PARAMS>\
	struct check_function_##funcname##_type {using type = decltype(funcname(chaos::meta::FakeInstance<PARAMS>()...));};\
}\
template<typename ...PARAMS>\
constexpr bool check_function_##funcname()\
{\
	return sizeof(details::check_function_##funcname##_helper(666, chaos::meta::FakeInstance<PARAMS>()...)) != 1;\
}\
\
template<typename ...PARAMS>\
constexpr bool check_function_##funcname##_v = check_function_##funcname<PARAMS...>();\
\
template<typename ...PARAMS>\
using typeof_function_##funcname = typename details::check_function_##funcname##_type<PARAMS...>::type;

	// ====================================================================================================
	// CHAOS_GENERATE_CHECK_METHOD_AND_FUNCTION(toto) generates 
	// ====================================================================================================

#define CHAOS_GENERATE_CHECK_METHOD_AND_FUNCTION(funcname)\
CHAOS_GENERATE_CHECK_METHOD(funcname)\
CHAOS_GENERATE_CHECK_FUNCTION(funcname)

	// ==================================================
	// Meta functions
	// ==================================================

	namespace meta
	{
		/** meta function to get a raw pointer from an input */
		template<typename T>
		T * get_raw_pointer(T * src)
		{
			return src;
		}
		/** meta function to get a raw pointer from an input */
		template<typename T>
		T const * get_raw_pointer(T const * src)
		{
			return src;
		}
		/** meta function to get a raw pointer from an input */
		template<typename T>
		T * get_raw_pointer(T & src)
		{
			return &src;
		}
		/** meta function to get a raw pointer from an input */
		template<typename T>
		T const * get_raw_pointer(T const & src)
		{
			return &src;
		}

		/** meta function to get a raw pointer from an input */
		template<typename T>
		 T * get_raw_pointer(shared_ptr<T> & src)
		{
			return src.get();
		}
		/** meta function to get a raw pointer from an input */
		template<typename T>
		T const * get_raw_pointer(shared_ptr<T> const & src)
		{
			return src.get();
		}

		/** meta function to get a raw pointer from an input */
		template<typename T>
		T * get_raw_pointer(std::unique_ptr<T> & src)
		{
			return src.get();
		}
		/** meta function to get a raw pointer from an input */
		template<typename T>
		T const * get_raw_pointer(std::unique_ptr<T> const & src)
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

		/** apply a functor on a boost::mpl::vector<>. Stop whenever some kind of result is found */
		template<typename ELEMENTS, typename FUNC, typename DEFAULT_RESULT>
		auto for_each(FUNC func, DEFAULT_RESULT default_result)
		{
			return details::for_each_internal<boost::mpl::begin<ELEMENTS>::type, boost::mpl::end<ELEMENTS>::type>(func, default_result);
		}

		/** apply a functor on a boost::mpl::vector<> */
		template<typename ELEMENTS, typename FUNC>
		auto for_each(FUNC func)
		{
			details::for_each_internal<boost::mpl::begin<ELEMENTS>::type, boost::mpl::end<ELEMENTS>::type>(func);
		}

		namespace details
		{

			/** internal method */
			template<typename IT, typename END, typename FUNC, typename DEFAULT_RESULT>
			auto for_each_internal(FUNC func, DEFAULT_RESULT default_result)
			{
				if constexpr (std::is_same_v<IT, END>) // constexpr is important for compilation
					return default_result;
				else
				{
					using VALUE = typename boost::mpl::deref<IT>::type;

					auto result = func(boost::mpl::identity<VALUE>());
					if (result)
						return result;
					return for_each_internal<boost::mpl::next<IT>::type, END>(func, default_result);
				}
			}

			/** internal method */
			template<typename IT, typename END, typename FUNC>
			void for_each_internal(FUNC func)
			{
				if constexpr (std::is_same_v<IT, END>)
					return;
				else
				{
					using VALUE = typename boost::mpl::deref<IT>::type;

					func(boost::mpl::identity<VALUE>());
					for_each_internal<boost::mpl::next<IT>::type, END>(func);
				}
			}

		}; // namespace details


	
	}; // namespace meta

}; // namespace chaos
