#include "Ludum43PCH.h"
#include "Ludum43Game.h"

CHAOS_GLOBAL_VARIABLE(bool, CheatMode);
CHAOS_GLOBAL_VARIABLE(float, CheatModeX);

int main(int argc, char ** argv, char ** env)
{


	return chaos::RunGame<LudumGame>(argc, argv, env);
}