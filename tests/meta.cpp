#include <stdio.h>
#include <stdlib.h>

#include <boost/mpl/if.hpp>
#include <boost/mpl/eval_if.hpp>
#include <boost/mpl/identity.hpp>
#include <boost/mpl/vector.hpp>
#include <boost/mpl/find.hpp>
#include <boost/mpl/find_if.hpp>
#include <boost/mpl/placeholders.hpp>
#include <boost/mpl/apply.hpp>
#include <boost/mpl/begin.hpp>
#include <boost/mpl/end.hpp>
#include <boost/mpl/not_equal_to.hpp>
#include <boost/mpl/comparison.hpp>
#include <boost/mpl/inherit_linearly.hpp>
#include <boost/mpl/inherit.hpp>
#include <boost/mpl/empty_base.hpp>
#include <boost/mpl/apply.hpp>
#include <boost/mpl/fold.hpp>
#include <boost/mpl/lambda.hpp>
#include <boost/mpl/next.hpp>
#include <boost/mpl/deref.hpp>
#include <boost/mpl/for_each.hpp>
#include <boost/mpl/apply_wrap.hpp>
#include <boost/mpl/has_xxx.hpp>
#include <boost/tti/has_member_data.hpp>
#include <boost/tti/has_member_function.hpp>
#include <boost/convert/detail/has_member.hpp>
#include <boost/mpl/aux_/has_type.hpp>



enum class MyEnum
{
    value1 = 2,
    value2 = 0,
    value3 = 2
};



class auto_instance
{
public:

	// no implementation : only a declaration
	template<typename T>
	static T & fake_instance()
	{
		static T instance;
		return instance;
	}
	
	
	template<typename T>
	operator T & ()
	{
		return fake_instance<T>();
	}
	
	
	template<typename T>
	operator T const & () const
	{
		return fake_instance<T>();
	}
	
	
};

class A
{
public:
	
	//int f(){ return 6;}
	void f(int * i){ }
	
	int * g(){return nullptr;}
	
	int trait = 0;
};

class B
{
public:
	
	void f(){ } // trap 
	
	void trait(){} // trap
};


class C
{
public:
	
	class f{ };
	
	class trait {};
};

// ==========================================================

char has_f(...)
{
	printf("FAILURE has_f\n");
	return 0;
}

// MARCHE PAS
//template<typename T, typename R>
//int f(T const & t, R param = decltype(T().f())())

// MARCHE PAS si f() retourne void
//template<typename T>
//int f(T const & t, decltype(T().f()) param = decltype(T().f())())

template<typename T>
int has_f(T const & t, decltype(T().f(T().g())) * param = auto_instance()) // the POINTER decltype(...) *  is IMPORTANT : we would not be able to work with 'void' returning functions
{
	printf("SUCCESS has_f 1\n");
	return 0;
}

template<typename T>
int has_f(T const & t, decltype(T().f()) * param = auto_instance())
{
	printf("SUCCESS has_f 2\n");
	return 0;
}

// ==========================================================

char has_trait(...)
{
	printf("FAILURE has_trait\n");
	return 0;
}

// COMPILATION ERROR
//template<typename T>
//int has_trait(T const & t, size_t s = sizeof(T::trait))


template<typename T, typename R = typename T::trait>
int has_trait(T const & t)
{
	printf("SUCCESS has_trait\n");
	return 0;	
}




int main(int argc, char **argv)
{
	has_f(A());
	has_f(B());
	has_f(C());
	
	has_trait(A());
	has_trait(B());
	has_trait(C());

	
	int u = auto_instance();

	
	return 0;
}
