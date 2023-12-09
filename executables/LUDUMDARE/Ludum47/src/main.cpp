#include "Ludum47PCH.h"
#include "Ludum47Game.h"

int main(int argc, char ** argv, char ** env)
{
	chaos::WindowPlacementInfo placement_info;

	return chaos::RunGame<LudumGame>(argc, argv, env, placement_info);
}