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

#if 0
/*


template<typename T1, typename T2>
using A = boost::mpl::inherit_linearly<
boost::mpl::vector<T1, T2>,
boost::mpl::inherit< 
boost::mpl::_1, 
boost::mpl::identity<boost::mpl::_2> 
> 
>;

*/

/***********************************************************************************************/

bool IsTrue(boost::mpl::true_)
{
	return true;
}

bool IsTrue(boost::mpl::false_)
{
	return false;
}

/***********************************************************************************************/


using MyTransf = boost::mpl::vector<
	chaos::add_logger<boost::mpl::_1>,
	chaos::add_logger<boost::mpl::_1>
>;



template<typename BEGIN, typename END, typename BASE>
class MyClassImpl : public MyClassImpl <
	typename boost::mpl::next<BEGIN>::type,
	END,
	typename boost::mpl::apply<
	typename boost::mpl::deref<BEGIN>::type,
	BASE
	>::type
> {};


template<typename IT, typename BASE>
class MyClassImpl < IT, IT, BASE > : public BASE {};

template<typename SEQ, typename BASE = chaos::EmptyClass>
using MyClass = MyClassImpl<
	typename boost::mpl::template begin<SEQ>::type,
	typename boost::mpl::template end<SEQ>::type,
	BASE>;


class T : public chaos::BaseClass<chaos::logger> {};

//class T : public chaos::add_logger<> {};


//class T : public cond_add_logger<has_logger_tag<boost::mpl::_1>, boost::mpl::_2>

//using TAGS = boost::mpl::vector<chaos::logger>;

//class T : public chaos::cond_add_logger<chaos::has_logger_tag<TAGS>, chaos::EmptyClass> {};
BOOST_DECLARE_HAS_MEMBER(has_xxx, xxx);

class A : public chaos::NamedObject
{

};

class B
{
public:

	typedef int type;
};

class C
{
public:

	void type() {}

};

class D
{
public:

	int type;

};

class MyApplication : public chaos::Application
{
protected:

	virtual bool Main() override
	{

		return true;
	}
};

int CHAOS_MAIN(int argc, char ** argv, char ** env)
{

	auto a0 = boost::mpl::aux::has_type<int>::value;
	auto a1 = boost::mpl::aux::has_type<A>::value;
	auto a2 = boost::mpl::aux::has_type<B>::value;
	auto a3 = boost::mpl::aux::has_type<C>::value;
	auto a4 = boost::mpl::aux::has_type<D>::value;


	auto b0 = boost::mpl::aux::has_type<int>::type();
	auto b1 = boost::mpl::aux::has_type<A>::type();
	auto b2 = boost::mpl::aux::has_type<B>::type();
	auto b3 = boost::mpl::aux::has_type<C>::type();
	auto b4 = boost::mpl::aux::has_type<D>::type();

	auto c0 = chaos::meta::get_type<int>::type();
	auto c1 = chaos::meta::get_type<A>::type();
	auto c2 = chaos::meta::get_type<B>::type();
	auto c3 = chaos::meta::get_type<C>::type();
	auto c4 = chaos::meta::get_type<D>::type();



	chaos::RunApplication<MyApplication>(argc, argv, env);
	return 0;
}



#endif



class A
{

};

class B : public A
{
	int xxx;

};

class C
{
	int xxx() 
	{
		return 0;
	}

};

class D
{
	class xxx {};

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


	auto f1 = X1::value;
	auto f2 = X2::value;
	auto f3 = X3::value;
	auto f4 = X4::value;

	return 0;
}


