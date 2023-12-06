#include "Ludum43PCH.h"
#include "Ludum43Game.h"

int main(int argc, char ** argv, char ** env)
{
	chaos::WindowCreateParams create_params;
	create_params.width = 500;
	create_params.height = 500;

	return chaos::RunGame<LudumGame>(argc, argv, env, create_params);
}