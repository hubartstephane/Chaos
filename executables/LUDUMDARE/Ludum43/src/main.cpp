#include "Ludum43PCH.h"
#include "Ludum43Game.h"

int main(int argc, char ** argv, char ** env)
{
	chaos::WindowParams params;
	params.monitor = nullptr;
	params.width = 500;
	params.height = 500;
	params.monitor_index = 0;

	chaos::GLFWWindowHints hints;

	return chaos::RunGame<LudumGame>(argc, argv, env, params, hints);
}


