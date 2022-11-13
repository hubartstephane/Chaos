#include "Ludum50PCH.h"
#include "Ludum50Game.h"

int main(int argc, char ** argv, char ** env)
{
	WindowCreateParams create_params;
	create_params.monitor = nullptr;
	create_params.width = 500;
	create_params.height = 500;
	create_params.monitor_index = 0;

	return RunGame<LudumGame>(argc, argv, env, create_params);
}