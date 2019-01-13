#include <chaos/StandardHeaders.h> 
#include <death/Death.h> 

#include "Ludum43Game.h"

int _tmain(int argc, char ** argv, char ** env)
{
	auto a = PROJECT_PATH;
	auto b = PROJECT_SRC_PATH;
	auto c = PROJECT_BUILD_PATH;

	return 0;

	chaos::MyGLFW::SingleWindowApplicationParams params;
	params.monitor = nullptr;
	params.width = 500;
	params.height = 500;
	params.monitor_index = 0;
	death::RunGame<LudumGame>(argc, argv, env, params);
	return 0;
}


