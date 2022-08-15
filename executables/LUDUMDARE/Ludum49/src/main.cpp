#include <chaos/Chaos.h>
#include "Ludum49Game.h"

int main(int argc, char ** argv, char ** env)
{
	WindowParams params;
	params.monitor = nullptr;
	params.width = 500;
	params.height = 500;
	params.monitor_index = 0;

	WindowHints hints;

	return RunGame<LudumGame>(argc, argv, env, params, hints);
}
