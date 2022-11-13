#include "Ludum46PCH.h"
#include "Ludum46Game.h"

int main(int argc, char ** argv, char ** env)
{
	chaos::WindowCreateParams create_params;
	create_params.monitor = nullptr;
	create_params.width = 500;
	create_params.height = 500;
	create_params.monitor_index = 0;

	return chaos::RunGame<LudumGame>(argc, argv, env, create_params);
}
