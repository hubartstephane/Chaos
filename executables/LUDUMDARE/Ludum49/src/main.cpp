#include "Ludum49PCH.h"
#include "Ludum49Game.h"

int main(int argc, char ** argv, char ** env)
{
	WindowCreateParams create_params;
	create_params.width = 500;
	create_params.height = 500;

	return RunGame<LudumGame>(argc, argv, env, create_params);
}