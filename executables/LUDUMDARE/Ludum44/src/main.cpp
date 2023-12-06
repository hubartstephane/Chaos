#include "Ludum44PCH.h"
#include "Ludum44Game.h"

int main(int argc, char ** argv, char ** env)
{
	chaos::WindowCreateParams create_params;
	create_params.width = 500;
	create_params.height = 500;

	return chaos::RunGame<LudumGame>(argc, argv, env, create_params);
}