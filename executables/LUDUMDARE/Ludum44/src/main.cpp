#include "Ludum44PCH.h"
#include "Ludum44Game.h"

int main(int argc, char ** argv, char ** env)
{
	chaos::WindowPlacementInfo placement_info;
	placement_info.size = { 500, 500 };

	return chaos::RunGame<LudumGame>(argc, argv, env, placement_info);
}