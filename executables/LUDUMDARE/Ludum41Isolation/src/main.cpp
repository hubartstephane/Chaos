#include <chaos/Chaos.h>

#include "Ludum41IsolationGame.h"

int CHAOS_MAIN(int argc, char ** argv, char ** env)
{
	chaos::MyGLFW::WindowParams params;
	params.monitor = nullptr;
	params.width = 500;
	params.height = 500;
	params.monitor_index = 0;

	chaos::MyGLFW::WindowHints hints;

	return chaos::RunGame<LudumGame>(argc, argv, env, params, hints);
}


