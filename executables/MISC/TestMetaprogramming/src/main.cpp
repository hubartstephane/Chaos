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

	void fff() {  }

};

class C
{
public:

	void fff(int) { }

};


class D
{
public:

	void fff(int, float) {  }
};

class E
{
public:


	template<typename ...PARAMS>
	void fff(PARAMS ...) {  }

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
	auto has_function_xxx_helper_no_params() -> decltype(chaos::meta::GenerateFakeInstance<T>().fff()) *;

	template<typename T>
	char has_function_xxx_helper_no_params(...);

	template<typename T, typename ...PARAMS>
	auto has_function_xxx_helper(PARAMS... params) -> decltype(chaos::meta::GenerateFakeInstance<T>().fff(params...)) *;

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








#define GENERATE_HAS_FUNCTION_SIGNATURE(funcname)\
namespace details\
{\
	template<typename T>\
	auto has_function_signature_##funcname##_helper_no_params(T * t) -> decltype(t->funcname()) *;\
	char has_function_signature_##funcname##_helper_no_params(...);\
	template<typename T, typename ...PARAMS>\
	auto has_function_signature_##funcname##_helper(T * t, PARAMS... params) -> decltype(t->funcname(params...)) *;\
	char has_function_signature_##funcname##_helper(...);\
}\
template<typename T, typename ...PARAMS>\
auto has_function_signature_##funcname##(T * t, PARAMS... params)\
{\
	return boost::mpl::bool_<sizeof(details::has_function_signature_##funcname##_helper(t, params...)) != 1>();\
}\
template<typename T>\
auto has_function_signature_##funcname##(T * t)\
{\
	return boost::mpl::bool_<sizeof(details::has_function_signature_##funcname##_helper_no_params(t)) != 1>();\
}




GENERATE_HAS_FUNCTION_SIGNATURE(fff)


int CHAOS_MAIN(int argc, char ** argv, char ** env)
{
	A a;
	B b;
	C c;
	D d;
	E e;

	auto a0 = has_function_signature_fff(&a);
	auto b0 = has_function_signature_fff(&b);
	auto c0 = has_function_signature_fff(&c);
	auto d0 = has_function_signature_fff(&d);
	auto e0 = has_function_signature_fff(&e);

	auto a1 = has_function_signature_fff(&a, 1);
	auto b1 = has_function_signature_fff(&b, 1);
	auto c1 = has_function_signature_fff(&c, 1);
	auto d1 = has_function_signature_fff(&d, 1);
	auto e1 = has_function_signature_fff(&e, 1);

	auto a2 = has_function_signature_fff(&a, 1, 3.3f);
	auto b2 = has_function_signature_fff(&b, 1, 3.3f);
	auto c2 = has_function_signature_fff(&c, 1, 3.3f);
	auto d2 = has_function_signature_fff(&d, 1, 3.3f);
	auto e2 = has_function_signature_fff(&e, 1, 3.3f);

	return 0;
}
