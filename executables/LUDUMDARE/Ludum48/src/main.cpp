#include "Ludum48PCH.h"
#include "Ludum48Game.h"

int main(int argc, char ** argv, char ** env)
{
	WindowCreateParams create_params;

	return RunGame<LudumGame>(argc, argv, env, placement_info);
}