#include "Ludum48PCH.h"
#include "Ludum48Game.h"

int main(int argc, char ** argv, char ** env)
{
	WindowCreateParams create_params;
	placement_info.size = { 500, 500 };

	return RunGame<LudumGame>(argc, argv, env, placement_info);
}