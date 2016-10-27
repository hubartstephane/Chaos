#include <chaos/SimpleWin32Class.h>
#include <chaos/SimpleWin32Window.h>
#include <chaos/WinTools.h>
#include <chaos/StringTools.h>
#include <chaos/MathTools.h>
#include <chaos/Application.h>

#include <chaos/Metaprogramming.h>
#include <chaos/EmptyClass.h>


/*
typedef boost::mpl::inherit_linearly<
	boost::mpl::vector<int,char const*,bool>
	, boost::mpl::inherit< boost::mpl::_1, tuple_field<_2> >
>::type tuple;
*/

template<typename T1, typename T2>
using A = boost::mpl::inherit_linearly<
	boost::mpl::vector<T1, T2>,
	boost::mpl::inherit< 
		boost::mpl::_1, 
		boost::mpl::identity<boost::mpl::_2> 
	> 
>;


/***********************************************************************************************/

class BB
{

};

class Base : public chaos::cond_add_vdestroy<boost::mpl::true_, BB>
{
public:

	~Base()
  {
    std::cout << "Base::destructor" << std::endl;
  }

};

class C : public Base
{
public:

	~C()
	{
		std::cout << "C::destructor" << std::endl;
	}
};

bool IsTrue(boost::mpl::true_)
{
	return true;
}

bool IsTrue(boost::mpl::false_)
{
	return false;
}

class X
{
public:

	X(){std::cout << "X" << std::endl;}
};
class Y
{
public:

	Y(){std::cout << "Y" << std::endl;}
};

/***********************************************************************************************/

int _tmain(int argc, char ** argv, char ** env)
{
  chaos::Application::Initialize<chaos::Application>(argc, argv, env);

  chaos::WinTools::AllocConsoleAndRedirectStdOutput();



	{
		class A : public chaos::cond_add_logger<boost::mpl::false_>{};

		delete(new A);
	
	
	}




	A<X, Y>::type t;




	chaos::Application::Finalize();
	return 0;





#if 0

  //BB * b = new chaos::ConditionnalAddVirtualDestructor<boost::mpl::true_, Base>;

	Base * b = new C;

  delete(b);

	b = nullptr;


	bool t1 = IsTrue(chaos::has_nocopy_tag<chaos::nocopy>::type());
	bool t2 = IsTrue(chaos::has_nocopy_tag<chaos::vdestroy>::type());

//  B b;
//  B b2;

	{

	using vec = boost::mpl::vector<chaos::EmptyClass, chaos::nocopy, int>;

	using has1 = chaos::meta::has_satisfying_element<vec, boost::is_same<int,boost::mpl::_> >;
	using has2 = chaos::meta::has_satisfying_element<vec, boost::is_same<float,boost::mpl::_> >;
	//using has3 = chaos::has_satisfying_element<vec, boost::mpl::identity<boost::mpl::true_ >>;
	//using has4 = chaos::has_satisfying_element<vec, boost::mpl::identity<boost::mpl::false_ >>;


	using has3 = chaos::meta::has_satisfying_element<vec, chaos::has_nocopy_tag<boost::mpl::_> >;
	using has4 = chaos::meta::has_satisfying_element<vec, chaos::has_vdestroy_tag<boost::mpl::_> >;

	bool t1 = IsTrue(has1::type());
	bool t2 = IsTrue(has2::type());
	bool t3 = IsTrue(has3::type());
	bool t4 = IsTrue(has4::type());
	//bool t3 = IsTrue(has3::type());
	//bool t4 = IsTrue(has4::type());

	int i = 0;
	++i;
	//using has1 = chaos::has_satisfying_element<vec, boost::mpl::true_>;
	//using has2 = chaos::has_satisfying_element<vec, boost::mpl::false_>;

	//bool t1 = IsTrue(has1::type());
	//bool t2 = IsTrue(has2());

#if 0

	using h1 = boost::is_same<
		boost::mpl::end,
		boost::mpl::find_if<vec, chaos::has_nocopy<boost::mpl::_>
		>;
	using h2 = boost::mpl::find_if<vec, chaos::has_nocopy<boost::mpl::_>>;

	bool t1 = IsTrue(h1::type());
	bool t2 = IsTrue(h2::type());
#endif
	}
	return 0;


#if 0

  chaos::NamedObjectArray<chaos::NamedObject> m1;
  chaos::NamedObjectArray<chaos::NamedObject*> m2;

  chaos::NamedObject * p1 = m1.GetElementByName(nullptr);
  chaos::NamedObject * p2 = m2.GetElementByName(nullptr);

  chaos::NamedObject * p3 = m1.GetElementByTag(0);
  chaos::NamedObject * p4 = m2.GetElementByTag(0);


	using b1 = chaos::meta::add_uniq_pointer<int *>;
	using b2 = chaos::meta::add_uniq_pointer<int>;

	b1 i;
	b2 ii;

#endif


#endif

}


