#include "Ludum41PCH.h"
#include "Ludum41Game.h"

int main(int argc, char ** argv, char ** env)
{
	return chaos::RunGame<LudumGame>(argc, argv, env);
}