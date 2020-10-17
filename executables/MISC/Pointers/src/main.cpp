#include <chaos/StandardHeaders.h>
#include <chaos/SmartPointers.h>
#include <chaos/Object.h>













class A : public chaos::Object
{
public:

	A()
	{
		static int k = 0;
		i = ++k;
	}

	~A()
	{
		int k = 0;
		++k;

	}


	int i = 0;

};

class B : public A {};



int CHAOS_MAIN(int argc, char ** argv, char ** env)
{

	chaos::DisableReferenceCount<A> aaa;
	{
		


		chaos::shared_ptr<A> s1 = &aaa;

		argc = argc;
	}

	argc = argc;


#if 1
	{
		chaos::shared_ptr<A> s1 = new A;



		//chaos::shared_ptr<A> s2 = std::move(s1);
		auto p1 = s1.get();

		chaos::weak_ptr<A> w1 = s1.get();


	//	chaos::weak_ptr<A> w2 = w1; // std::move(w1);

		auto p2 = w1.get();

		s1 = nullptr;

		auto p3 = s1.get();
		auto p4 = w1.get();
		//auto p5 = w2.get();


		argc = argc;
	}


	argc = argc;
#endif










#if 0
	{
		toto::shared_ptr<A> s1 = new A;



		//toto::shared_ptr<A> s2 = std::move(s1);
		auto p1 = s1.get();

		toto::weak_ptr<A> w1 = s1.get();


		toto::weak_ptr<A> w2 = w1; // std::move(w1);

		auto p2 = w1.get();

		s1 = nullptr;

		auto p3 = s1.get();
		auto p4 = w1.get();
		//auto p5 = w2.get();


		argc = argc;
	}


	argc = argc;
#endif

  return 0;
}


