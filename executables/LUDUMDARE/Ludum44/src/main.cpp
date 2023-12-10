#include "Ludum44PCH.h"
#include "Ludum44Game.h"

int main(int argc, char ** argv, char ** env)
{
	return chaos::RunGame<LudumGame>(argc, argv, env);
}