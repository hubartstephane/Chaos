#include <chaos/StandardHeaders.h> 
#include <chaos/Metaprogramming.h> 
#include <death/Death.h> 


#include <boost/tti/has_member_function.hpp>

#include "Ludum46Game.h"





CHAOS_GENERATE_HAS_CALLABLE_FUNCTION(toto);

class A
{
public:

	int toto() const { return 1; }

	int toto(int) { return 2; }

	int toto(double) { return 3; }

	int toto(A *, A *) const { return 4; }


};

class B
{
	
};


int CHAOS_MAIN(int argc, char ** argv, char ** env)
{	
	//using namespace chaos::meta;
	bool a1 = has_callable_toto<A>();
	bool a2 = has_callable_toto<A>(3);
	bool a3 = has_callable_toto<A>(4.0f);
	bool a4 = has_callable_toto<A>(4.0);
	bool a5 = has_callable_toto<A>(glm::vec2(0.0f, 0.0f));

	bool b1 = has_callable_toto<A const>();
	bool b2 = has_callable_toto<A const>(3);
	bool b3 = has_callable_toto<A const>(4.0f);
	bool b4 = has_callable_toto<A const>(4.0);
	bool b5 = has_callable_toto<A const>(glm::vec2(0.0f, 0.0f));
	


	int i = 0;
	if (0)
	{

		++i;
		


	}
	else
	{
		--i;

	}



	chaos::MyGLFW::SingleWindowApplicationParams params;
	params.monitor = nullptr;
	params.width = 500;
	params.height = 500;
	params.monitor_index = 0;
	death::RunGame<LudumGame>(argc, argv, env, params);
	return 0;
}
