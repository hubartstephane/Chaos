#include <chaos/SimpleWin32Class.h>
#include <chaos/SimpleWin32Window.h>
#include <chaos/WinTools.h>
#include <chaos/StringTools.h>
#include <chaos/MathTools.h>
#include <chaos/Application.h>

#include <chaos/Metaprogramming.h>
#include <chaos/EmptyClass.h>
#include <chaos/NamedObject.h>
#include <chaos/ClassTools.h>

class A
{
public:

	

};

class B
{
public:

	int fff() { return 2; }

};

class C
{
public:

	int fff(int) { return 3; }

};


class D
{
public:

	int fff(int, float) { return 4; }
};

class E
{
public:


	template<typename ...PARAMS>
	int fff(PARAMS... params) { return 0; }

	//template<typename T, typename V>
	//int fff(T t, V v) { return 0; }
};

#if 0 // WORKING


template<typename T>
auto f(T & t, int a) -> decltype(chaos::meta::GenerateFakeInstance<T>().fff(a));

char f(...);

template<typename T>\
using has_f = boost::mpl::bool_<\
	sizeof(f(chaos::meta::GenerateFakeInstance<T>(), 3)) != 1\
>;


#endif


// WORKING

#if 0

template<typename T>
auto f(int a) -> decltype(chaos::meta::GenerateFakeInstance<T>().fff(a));

template<typename T>
char f(...);

template<typename T>\
using has_f = boost::mpl::bool_<\
	sizeof(f<T>(3)) != 1\
>;

#endif 0


// WORKING

#if 0

template<typename T, typename ...PARAMS>
auto f(PARAMS... params) -> decltype(chaos::meta::GenerateFakeInstance<T>().fff(params...));

template<typename T>
char f(...);

template<typename T>\
using has_f = boost::mpl::bool_<\
	sizeof(f<T>(3, 6.6)) != 1\
>;

#endif








namespace details
{
	template<typename T>
	auto has_function_xxx_helper_no_params() -> decltype(chaos::meta::GenerateFakeInstance<T>().fff());

	template<typename T>
	char has_function_xxx_helper_no_params(...);

	template<typename T, typename ...PARAMS>
	auto has_function_xxx_helper(PARAMS... params) -> decltype(chaos::meta::GenerateFakeInstance<T>().fff(params...));

	template<typename T>
	char has_function_xxx_helper(...);
}



template<typename T, typename ...PARAMS>
auto has_function_xxx(PARAMS... params)
{
	return boost::mpl::bool_<sizeof(details::has_function_xxx_helper<T>(params...)) != 1>();
}

template<typename T>
auto has_function_xxx()
{
	return boost::mpl::bool_<sizeof(details::has_function_xxx_helper_no_params<T>()) != 1>();
}



int CHAOS_MAIN(int argc, char ** argv, char ** env)
{
	A a;
	B b;
	C c;
	D d;
	E e;

	auto a0 = has_function_xxx<A>();
	auto b0 = has_function_xxx<B>();
	auto c0 = has_function_xxx<C>();
	auto d0 = has_function_xxx<D>();
	auto e0 = has_function_xxx<E>();

	auto a1 = has_function_xxx<A>(1);
	auto b1 = has_function_xxx<B>(1);
	auto c1 = has_function_xxx<C>(1);
	auto d1 = has_function_xxx<D>(1);
	auto e1 = has_function_xxx<E>(1);

	auto a2 = has_function_xxx<A>(1, 3.3f);
	auto b2 = has_function_xxx<B>(1, 3.3f);
	auto c2 = has_function_xxx<C>(1, 3.3f);
	auto d2 = has_function_xxx<D>(1, 3.3f);
	auto e2 = has_function_xxx<E>(1, 3.3f);

	return 0;
}

// =========================================

#if 0

#if 0


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

#endif



namespace details
{
	char has_function_helper_xxx(...);
	template<typename T>
	auto has_function_helper_xxx(T const & t) -> decltype(&T::xxx);
};

template<typename T>
using has_function_xxx = boost::mpl::bool_<
	sizeof(details::has_function_helper_xxx(chaos::meta::GenerateFakeInstance<T>())) != 1
>;

namespace details
{
	char has_member_helper_xxx(...);
	template<typename T>
	auto has_member_helper_xxx(T const & t) -> decltype(&t.xxx);
};

template<typename T>
using has_member_xxx = boost::mpl::bool_<
	sizeof(details::has_member_helper_xxx(chaos::meta::GenerateFakeInstance<T>())) != 1
>;


namespace details
{
	template<typename ...PARAMS>
	char CanCallImplementation(...);

	template<typename T, typename ...PARAMS>
	auto CanCallImplementation(T & t, PARAMS... params) -> decltype(&t.fff(params...));
}



template<typename T, typename ...PARAMS>
using CanCall = boost::mpl::bool_<
	sizeof(details::CanCallImplementation(chaos::meta::GenerateFakeInstance<T>())) != 1
>;




struct A
{
	void fff(){}

};

struct B : public A
{
	int xxx;

	void fff(int){}

};

struct C
{
	int xxx() 
	{
		return 0;
	}

	void fff(int, float){}

};

struct D
{
	class xxx {};

};

struct E
{
	static int xxx;

};

BOOST_DECLARE_HAS_MEMBER(has_xxx, xxx);

int CHAOS_MAIN(int argc, char ** argv, char ** env)
{
	auto a = chaos::ClassTools::GetClassID<A>();
	auto b = chaos::ClassTools::GetClassID<A const>();

	type_info const & X = typeid(A);

	auto ffA = has_xxx<A>::value;
	auto ffB = has_xxx<B>::value;

	using X1 = boost::mpl::bool_<has_xxx<A>::value>; // no data => false
	using X2 = boost::mpl::bool_<has_xxx<B>::value>; // data    => true
	using X3 = boost::mpl::bool_<has_xxx<C>::value>; // member function => true !!!
	using X4 = boost::mpl::bool_<has_xxx<D>::value>; // nested class => true !!!
	using X5 = boost::mpl::bool_<has_xxx<E>::value>; // static data


	auto f1 = X1::value;
	auto f2 = X2::value;
	auto f3 = X3::value;
	auto f4 = X4::value;
	auto f5 = X5::value;








	auto b1 = has_function_xxx<A>::value;
	auto b2 = has_function_xxx<B>::value;
	auto b3 = has_function_xxx<C>::value;
	auto b4 = has_function_xxx<D>::value;
	auto b5 = has_function_xxx<E>::value;


	auto c1 = has_member_xxx<A>::value;
	auto c2 = has_member_xxx<B>::value;
	auto c3 = has_member_xxx<C>::value;
	auto c4 = has_member_xxx<D>::value;
	auto c5 = has_member_xxx<E>::value;

	

	auto d1 = CanCall<A>::value;
	auto d2 = CanCall<B>::value;
	auto d3 = CanCall<C>::value;
	auto d4 = CanCall<D>::value;
	auto d5 = CanCall<E>::value;


	return 0;
}


#endif