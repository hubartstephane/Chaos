#include "Ludum45PCH.h"
#include "Ludum45Game.h"

int main(int argc, char ** argv, char ** env)
{
	chaos::WindowPlacementInfo placement_info;
	placement_info.size = { 500, 500 };
	placement_info.monitor_index = 0;

	return chaos::RunGame<LudumGame>(argc, argv, env, placement_info);
}
