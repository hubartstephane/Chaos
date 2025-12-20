#include "Ludum43PCH.h"
#include "Ludum43Game.h"

int main(int argc, char ** argv, char ** env)
{
	return chaos::RunGame<LudumGame>(argc, argv, env);
}