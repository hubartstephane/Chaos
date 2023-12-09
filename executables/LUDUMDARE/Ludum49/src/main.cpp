#include "Ludum49PCH.h"
#include "Ludum49Game.h"

int main(int argc, char ** argv, char ** env)
{
	WindowCreateParams create_params;

	return RunGame<LudumGame>(argc, argv, env, placement_info);
}