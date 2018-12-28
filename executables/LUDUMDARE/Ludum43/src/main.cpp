#include <chaos/StandardHeaders.h> 

#include <death/Death.h> 

#include "Ludum43Game.h"

#include <boost/enable_shared_from_this.hpp>
#include <boost/shared_ptr.hpp>

int _tmain(int argc, char ** argv, char ** env)
{
	chaos::MyGLFW::SingleWindowApplicationParams params;
	params.monitor = nullptr;
	params.width = 500;
	params.height = 500;
	params.monitor_index = 0;
	death::RunGame<LudumGame>(argc, argv, env, params);
	return 0;
}


