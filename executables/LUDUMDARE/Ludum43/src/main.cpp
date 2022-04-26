#include <chaos/Chaos.h>

#include "Ludum43Game.h"

import MyClass;

int CHAOS_MAIN(int argc, char ** argv, char ** env)
{
	MyClass c;
	int ii = c.f();

	chaos::WindowParams params;
	params.monitor = nullptr;
	params.width = 500;
	params.height = 500;
	params.monitor_index = 0;

	chaos::WindowHints hints;

	return chaos::RunGame<LudumGame>(argc, argv, env, params, hints);
}


