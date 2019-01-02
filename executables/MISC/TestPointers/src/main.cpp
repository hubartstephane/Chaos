#include <chaos/StandardHeaders.h>
#include <chaos/SmartPointers.h>
#include <chaos/ReferencedObject.h>





































class A : public chaos::ReferencedObject
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



int _tmain(int argc, char ** argv, char ** env)
{
#if 0

	{
		chaos::shared_ptr<A> a = new A;
		chaos::shared_ptr<A> b = nullptr;

		chaos::shared_ptr<B> c = nullptr;


		a = c;


		argc = argc;
		b = a;
		argc = argc;

		std::swap(a, b);

		argc = argc;

	}
#endif



  return 0;
}


