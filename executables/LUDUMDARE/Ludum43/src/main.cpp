#include <chaos/StandardHeaders.h> 
#include <death/Death.h> 
#include "Ludum43Game.h"

#include <chaos/EmptyClass.h>
#include <chaos/MetaProgramming.h>


CHAOS_GENERATE_HAS_TRAIT(toto)

class A
{

};

class B
{
public:

	class toto {};

};




int CHAOS_MAIN(int argc, char ** argv, char ** env)
{	
	auto a = has_toto<A>::type();
	auto b = has_toto<B>::type();

	auto c = get_toto<A>::type();
	auto d = get_toto<B>::type();


	chaos::MyGLFW::SingleWindowApplicationParams params;
	params.monitor = nullptr;
	params.width = 500;
	params.height = 500;
	params.monitor_index = 0;
	death::RunGame<LudumGame>(argc, argv, env, params);
	return 0;
}


