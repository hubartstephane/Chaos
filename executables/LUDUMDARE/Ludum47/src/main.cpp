#include <chaos/Chaos.h>

#include "Ludum47Game.h"

int CHAOS_MAIN(int argc, char ** argv, char ** env)
{
	chaos::MyGLFW::SingleWindowApplicationParams params;
	params.monitor = nullptr;
	params.width = 500;
	params.height = 500;
	params.monitor_index = 0;
	chaos::RunGame<LudumGame>(argc, argv, env, params);
	return 0;
}
