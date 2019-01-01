#include <chaos/StandardHeaders.h> 

#include <death/Death.h> 

#include "Ludum43Game.h"

#include <chaos/ReferencedObject.h>
#include <chaos/SmartPointers.h>



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


	chaos::MyGLFW::SingleWindowApplicationParams params;
	params.monitor = nullptr;
	params.width = 500;
	params.height = 500;
	params.monitor_index = 0;
	death::RunGame<LudumGame>(argc, argv, env, params);
	return 0;
}


