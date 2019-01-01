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


int _tmain(int argc, char ** argv, char ** env)
{
	chaos::shared_ptr<A> a = new A;
	chaos::shared_ptr<A> b = nullptr;

	std::swap(a, b);

	argc = argc;

	{
		std::vector<chaos::shared_ptr<A>> v;

		for (int i = 0; i < 8; ++i)
			v.push_back(new A);

		argc = argc;
	}


//	return 0;

	chaos::MyGLFW::SingleWindowApplicationParams params;
	params.monitor = nullptr;
	params.width = 500;
	params.height = 500;
	params.monitor_index = 0;
	death::RunGame<LudumGame>(argc, argv, env, params);
	return 0;
}


