#include <chaos/StandardHeaders.h> 
#include <death/Death.h> 

#include "Ludum46Game.h"

class A
{
public:

	virtual ~A() {}


};

class B : public A
{
	


};


int CHAOS_MAIN(int argc, char ** argv, char ** env)
{
	A a;
	B b;
	A a2;
	B b2;

	uintptr_t pa = *((uintptr_t*)&a);
	uintptr_t pb = *((uintptr_t*)&b);
	uintptr_t pa2 = *((uintptr_t*)&a2);
	uintptr_t pb2 = *((uintptr_t*)&b2);




	return 0;

	chaos::MyGLFW::SingleWindowApplicationParams params;
	params.monitor = nullptr;
	params.width = 500;
	params.height = 500;
	params.monitor_index = 0;
	death::RunGame<LudumGame>(argc, argv, env, params);
	return 0;
}
